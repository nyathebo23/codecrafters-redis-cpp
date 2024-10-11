#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <variant>
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
#include "decode/simple_data_parser_dec.h"
#include "socket_management.h"



void SocketManagement::erase_key(const std::string& key) {
    this->dict_data.erase(key);
}

void SocketManagement::execute_after_delay(int delay, const std::string& key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    this->erase_key(key);
}

void SocketManagement::handle_connection(int& clientfd){
    
    while (1) {
        char buffer[128];    
        if (recv(clientfd, &buffer, sizeof(buffer), 0) <= 0) {
            close(clientfd);
            return;
        }
        std::string data(buffer);
        ArrayResp arr_resp = parse_decode_array(data);
        auto arr = std::get<ArrayAndInd>(arr_resp.first);
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
                for (int replica_fd: replicas_fd) {
                    if (send(replica_fd, data.c_str(), data.length(), 0) <= 0)
                        std::cout <<  "replica send msg failed";                
                }
                if (vals.size() > 2){
                    std::string key = std::any_cast<std::string>(vals[1]);
                    this->dict_data[key] = std::any_cast<std::string>(vals[2]);
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
                    auto keys_values = get_keys_values_from_file(this->extra_args["dir"] + "/" + this->extra_args["dbfilename"]);
                    int index = 0, size = keys_values.first.size();
                    while (index < size && std::any_cast<std::string>(keys_values.first[index]) != key){
                        index++;
                    }
                    if (index >= size || size == 0){
                        if (this->dict_data.count(key) == 0)
                            res = "$-1\r\n";
                        else 
                            res = parse_encode_bulk_string(this->dict_data[key]);
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
                    std::vector<std::any> values = {key, this->extra_args["dir"]};
                    res = parse_encode_array(values);
                }
                else if (key == "dbfilename"){
                    std::vector<std::any> values = {key, this->extra_args["dbfilename"]};
                    res = parse_encode_array(values);          
                }
            }
            }
            else if (cmd == "keys"){
                std::string param = std::any_cast<std::string>(vals[1]);
                //std::transform(param.begin(), param.end(), param.begin(), ::tolower);
                if (param == "*"){
                    auto keys_values = get_keys_values_from_file(this->extra_args["dir"] + "/" + this->extra_args["dbfilename"]);
                    auto keys = keys_values.first;
                    res = parse_encode_array(keys);
                }
            }
            else if (cmd == "info"){
                std::string param = std::any_cast<std::string>(vals[1]);
                if (param == "replication"){
                    std::string role = "master";
                    if (this->extra_args.count("replicaof") != 0){
                        role = "slave";
                    }
                    std::string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
                    int replication_offset = 0;
                    std::string str = "role:"+role + "\n"+ "master_replid:"+replication_id + 
                        "\n" + "master_repl_offset:"+ std::to_string(replication_offset);
                    res = parse_encode_bulk_string(str);
                }
            }
            else if (cmd == "replconf"){
                std::string param = std::any_cast<std::string>(vals[1]);
                if (param == "listening-port" || param == "capa" && vals.size() > 2){
                    res = parse_encode_simple_string(std::string("OK"));
                }
            }
            else if (cmd == "psync"){
                std::string param1 = std::any_cast<std::string>(vals[1]);
                std::string param2 = std::any_cast<std::string>(vals[2]);
                if (param1 == "?" && param2 == "-1"){
                    std::string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
                    res = parse_encode_simple_string("FULLRESYNC " + replication_id + " 0");
                }
            }
            if (!res.empty()){
                //std::cout <<  "azertyuiiopqsddfghj\n";
                if (send(clientfd, res.c_str(), res.length(), 0) <= 0)
                    std::cout <<  "send msg failed";
                if (cmd == "psync"){                                        
                    std::string file_content_str = "524544495330303131fa0972656469732d76657205372e322e30fa0a72656469732d62697473c040fa056374696d65c26d08bc65fa08757365642d6d656dc2b0c41000fa08616f662d62617365c000fff06e3bfec0ff5aa2";
                    std::vector<unsigned char> bytes = string_to_binary(file_content_str);
                    const size_t size = bytes.size();
                    unsigned char data[size];
                    memcpy(data, bytes.data(), size);
                    if (send(clientfd, data, size, 0) <= 0)
                        std::cout <<  "send msg failed";
                    else
                        replicas_fd.push_back(clientfd);
                }
            }
                
        }
    }
}

SocketManagement::SocketManagement(short family, int type, std::map<std::string, std::string> extra) {
    server_fd = socket(family, type, 0); 
    server_addr.sin_family = family;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    extra_args = extra;
    if (extra_args.count("host") != 0){
        std::string ip_address = extra_args["host"] == "localhost" ? "127.0.0.1" : extra_args["host"];
        inet_pton(family, ip_address.c_str(), &(this->server_addr.sin_addr));
    }
    int port = 6379;
    if (extra_args.count("port") != 0){
        port = std::stoi(extra_args["port"]);
    }
    server_addr.sin_port = htons(port);
}

int SocketManagement::get_server_fd() const{
    return server_fd;
}

struct sockaddr_in SocketManagement::get_server_addr() const {
    return server_addr;
}

int SocketManagement::socket_bind() {
    return bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
}

int SocketManagement::socket_listen(int connection_backlog){
    return listen(server_fd, connection_backlog);
}

int SocketManagement::send_receive_msg_by_command(std::string tosend, std::string toreceive){
    if (send(server_fd, tosend.c_str(), tosend.length(), 0) <= 0){
        std::cout << "Send "+ tosend + " handshake failed";
        close(server_fd);
        return -1;
    }
    char buffer[128];    
    if (recv(server_fd, &buffer, sizeof(buffer), 0) <= 0) {
        close(server_fd);
        return -1;
    }
    std::string data(buffer);
    std::string data_decoded = parse_decode_simple_string(data).first;
    // if (data_decoded != toreceive){
    //     std::cout << "Bad message receive to " + tosend + " which is " + data_decoded;
    //     return -1;
    // }
    return 1;
};


void SocketManagement::check_incoming_clients_connections(){
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";
  while (1){
      int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len); 
      std::cout << "Client connected\n";
      std::thread connection([this](int clientfd){handle_connection(clientfd);}, client_fd);
      connection.detach();
  }
  close(server_fd);
}

void SocketManagement::process_command(std::string data) {

    ArrayResp arr_resp = parse_decode_array(data);
    try {
        auto arr = std::get<ArrayAndInd>(arr_resp.first);
        auto vals = arr.first;
        if (vals[0].type() == typeid(std::string)){
            std::string cmd = std::any_cast<std::string>(vals[0]);
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
            std::string res;                
            if (cmd == "set"){
                if (vals.size() > 2){
                    std::string key = std::any_cast<std::string>(vals[1]);
                    this->dict_data[key] = std::any_cast<std::string>(vals[2]);
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
        }

    }
    catch(std::bad_variant_access e){
        auto arr = std::get<std::string>(arr_resp.first);
        std::cout << arr + " " + data;
      
    }

}

void SocketManagement::retrieve_commands_from_master() {
    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cout << "Connect to master failed";
        return;
    }
    while (1){
        std::cout << "azertyuiohggfgf";
        char buffer[128];    
        recv(server_fd, &buffer, sizeof(buffer), 0);
        // if (recv(server_fd, &buffer, sizeof(buffer), 0) <= 0) {
        //     close(server_fd);
        //     break;
        // } 
        std::string data(buffer);
        int pos = 0;
        int end = data.find("*", 1);
        while (end != std::string::npos){
            process_command(data.substr(pos, end-pos));
            pos = end;
            end = data.find("*", pos+1);
        }
        process_command(data.substr(pos));
    }
};


int SocketManagement::send_handshake_to_master(int port){
    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cout << "Connect to master failed";
        return -1;
    }
    std::vector<std::any> ping = {std::string("PING")};
    if(send_receive_msg_by_command(parse_encode_array(ping), "PONG") < 0)
        return -1;

    std::vector<std::any> replconf_msg1 = {std::string("REPLCONF"), std::string("listening-port"), std::to_string(port)}; 
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg1), "OK") < 0)
        return -1;
    
    std::vector<std::any> replconf_msg2 = {std::string("REPLCONF"), std::string("capa"), std::string("psync2")};
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg2), "OK") < 0)
        return -1;

    std::vector<std::any> psync_msg = {std::string("PSYNC"), std::string("?"), std::string("-1")};
    if(send_receive_msg_by_command(parse_encode_array(psync_msg), "FULLRESYNC <REPL_ID> 0") < 0)
        return -1;
    char buffer[128];
    if (recv(server_fd, &buffer, sizeof(buffer), 0) <= 0) {
            std::cout << "azertyuiohggfgf";
    }    
    retrieve_commands_from_master();
    return 1;
}
