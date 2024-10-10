#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/array_parser_dec.h"
#include "decode/small_aggregate_parser_dec.h"
#include "process_rdbfile_datas.h"

class SocketManagement {
    private:
        int server_fd;
        int client_fd;
        struct sockaddr_in server_addr;
        std::map<std::string, std::string> extra_args;
        std::map<std::string, std::string> dict_data;

        void erase_key(const std::string& key) {
            dict_data.erase(key);
        }

        void execute_after_delay(int delay, const std::string& key) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            this->erase_key(key);
        }

        sockaddr_in get_addr_from_params_datas(std::string master_raw_data){
            std::string dest_ip = master_raw_data.substr(0, master_raw_data.find_first_of(" "));
            std::string dest_port = master_raw_data.substr(master_raw_data.find_first_of(" ")+1);
            sockaddr_in master_addr;
            master_addr.sin_family = server_addr.sin_family;
            if (dest_ip == "localhost")
                master_addr.sin_addr.s_addr = INADDR_ANY;
            else {
                master_addr.sin_addr.s_addr = INADDR_ANY;
            }
            master_addr.sin_port = std::stoi(dest_port);
            return master_addr;
        }

        void handle_connection(){
            while (1) {
                char buffer[128];    
                if (recv(client_fd, &buffer, sizeof(buffer), 0) <= 0) {
                close(client_fd);
                return;
                }
                std::string data(buffer);
                ArrayResp arresp = parse_decode_array(data);
                auto arr = std::get<ArrayAndInd>(arresp.first);
                auto vals = arr.first;
                if (vals[0].type() == typeid(std::string)){
                    std::string cmd = std::any_cast<std::string>(vals[0]);
                    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
                    std::string res;
                    if (cmd == "echo"){
                        if (vals.size() == 2 && vals[1].type() == typeid(std::string)){
                        res = parse_encode_bulk_string(std::any_cast<std::string>(vals[1]));
                        }
                    }
                    else if (cmd == "ping"){
                        if (vals.size() == 1){
                        res = parse_encode_bulk_string("PONG");
                        }
                    }
                    else if (cmd == "set"){
                        if (vals.size() > 2){
                        std::string key = std::any_cast<std::string>(vals[1]);
                        dict_data[key] = std::any_cast<std::string>(vals[2]);
                        res = parse_encode_simple_string("OK");
                        if (vals.size() == 5 && vals[3].type() == typeid(std::string)){
                            std::string param = std::any_cast<std::string>(vals[3]);
                            std::transform(param.begin(), param.end(), param.begin(), ::tolower);
                            if (param == "px"){
                                const int duration = std::stoi(std::any_cast<std::string>(vals[4]));
                                std::thread t([this, &duration, &key]() {execute_after_delay(duration, key);});
                                t.detach();
                            }
                        }
                        }
                    }
                    else if (cmd == "get"){
                        if (vals.size() == 2 && vals[1].type() == typeid(std::string)){
                            std::string key = std::any_cast<std::string>(vals[1]);
                            auto keys_values = get_keys_values_from_file(extra_args["--dir"] + "/" + extra_args["--dbfilename"]);
                            int index = 0, size = keys_values.first.size();
                            while (index < size && std::any_cast<std::string>(keys_values.first[index]) != key){
                                index++;
                            }
                            if (index >= size || size == 0){
                                if (dict_data.count(key) == 0)
                                    res = "$-1\r\n";
                                else 
                                    res = parse_encode_bulk_string(dict_data[key]);
                            }
                            else
                                res = parse_encode_bulk_string(std::any_cast<std::string>(keys_values.second[index]));
                        }
                    }
                    else if (cmd == "config"){
                    std::string cmd2 = std::any_cast<std::string>(vals[1]);
                    std::transform(cmd2.begin(), cmd2.end(), cmd2.begin(), ::tolower);
                    if (cmd2 == "get"){
                        std::string key = std::any_cast<std::string>(vals[2]);
                        if (key == "dir"){
                            std::vector<std::any> values = {key, extra_args["--dir"]};
                            res = parse_encode_array(values);
                        }
                        else if (key == "dbfilename"){
                            std::vector<std::any> values = {key, extra_args["--dbfilename"]};
                            res = parse_encode_array(values);          
                        }
                    }
                    }
                    else if (cmd == "keys"){
                        std::string param = std::any_cast<std::string>(vals[1]);
                        //std::transform(param.begin(), param.end(), param.begin(), ::tolower);
                        if (param == "*"){
                            auto keys_values = get_keys_values_from_file(extra_args["--dir"] + "/" + extra_args["--dbfilename"]);
                            auto keys = keys_values.first;
                            res = parse_encode_array(keys);
                        }
                    }
                    else if (cmd == "info"){
                        std::string param = std::any_cast<std::string>(vals[1]);
                        if (param == "replication"){
                            std::string role = "master";
                            if (extra_args.count("--replicaof") != 0){
                                role = "slave";
                                std::string msg = parse_encode_bulk_string(std::string("PING"));
                                this->send_message_to_server(this->get_addr_from_params_datas(extra_args["--replicaof"]), msg);
                            }
                                
                            std::string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
                            int replication_offset = 0;
                            std::string str = "role:"+role + "\n"+ "master_replid:"+replication_id + 
                                "\n" + "master_repl_offset:"+ std::to_string(replication_offset);
                            res = parse_encode_bulk_string(str);
                        }
                    }
                    if (!res.empty())
                        send(client_fd, res.c_str(), res.length(), 0);
                }
            }
        }

    public:        
        SocketManagement(short family, int type, std::map<std::string, std::string> extra) {
           server_fd = socket(family, type, 0); 
           server_addr.sin_family = family;
           server_addr.sin_addr.s_addr = INADDR_ANY;
           extra_args = extra;
           int port = 6379;
           if (extra_args.count("--port") != 0){
                port = std::stoi(extra_args["--port"]);
           }
           server_addr.sin_port = htons(port);
        }

        int get_server_fd() const{
            return server_fd;
        }

        int socket_bind() {
            return bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        }

        int socket_listen(int connection_backlog){
            return listen(server_fd, connection_backlog);
        }

        void check_incoming_clients_connections(){
            sockaddr_in client_addr;
            int client_addr_len = sizeof(client_addr);
            std::cout << "Waiting for a client to connect...\n";
            while (1){
                client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len); 
                std::cout << "Client connected\n";
                std::thread connection([this](){handle_connection();});
                connection.detach();
            }
            close(server_fd);
        }

        int send_message_to_server(sockaddr_in dest_address, std::string msg){
            if (connect(server_fd, (struct sockaddr*)&dest_address, sizeof(dest_address)) < 0)
                return -1;
            return send(server_fd, msg.c_str(), msg.length(), 0);
        }

        
};