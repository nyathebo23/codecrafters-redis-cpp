#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <variant>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <sys/select.h>
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
#include "command_processing.h"
#include "stream_commands_processing.h"
#include "list_commands_processing.h"
#include "../globals_datas/global_datas.h"
#include "resp_constants.h"
#include <iomanip>

std::vector<std::string> SocketManagement::lower_str_params_decoded(std::vector<DecodedResult*> params) {
    std::vector<std::string> result;
    for (DecodedResult* param: params) {
        std::string param_str = param.asString();
        std::transform(param_str.begin(), param_str.end(), param_str.begin(), ::tolower);
        result.push_back(param_str);
    }
    return result;
}

std::string SocketManagement::run_command(std::string cmd, std::vector<DecodedResult*> extra_params, std::string data, int clientfd){
    if (cmd == "echo"){
        return CommandProcessing::echo(lower_str_params_decoded(extra_params));
    }
    else if (cmd == "wait"){
        return CommandProcessing::wait(extra_params[0].asInteger(), extra_params[1].asInteger());
    }
    else if (cmd == "ping"){
        return CommandProcessing::ping();
    }
    else if (cmd == "incr"){
        return CommandProcessing::incr(extra_params[0]);
    }
    else if (cmd == "type"){
        return CommandProcessing::type(extra_params[0]);
    }
    else if (cmd == "xadd"){
        return StreamCommandsProcessing::xadd(lower_str_params_decoded(extra_params));
    }
    else if (cmd == "xrange"){
        return StreamCommandsProcessing::xrange(lower_str_params_decoded(extra_params));
    }
    else if (cmd == "xread"){
        std::vector<std::string>> extras = lower_str_params_decoded(extra_params);
        if (extras[0] == "block")
            return StreamCommandsProcessing::xread_with_block(extras);
        else
            return StreamCommandsProcessing::xread(extras);
    }
    else if (cmd == "rpush") {
        return ListCommandsProcessing::rpush(lower_str_params_decoded(extra_params));
    }
    else if (cmd == "lrange") {
        return ListCommandsProcessing::lrange(extra_params);
    }
    else if (cmd == "set") {
        return CommandProcessing::set(lower_str_params_decoded(extra_params), data);
    }
    else if (cmd == "get"){
        return CommandProcessing::get(lower_str_params_decoded(extra_params), extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "config"){
        return CommandProcessing::config(lower_str_params_decoded(extra_params), extra_args);
    }
    else if (cmd == "keys"){
        return CommandProcessing::keys(lower_str_params_decoded(extra_params), extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "info"){
        return CommandProcessing::info(lower_str_params_decoded(extra_params), GlobalDatas::isMaster ? "master" : "slave");
    }
    else if (cmd == "replconf"){
        return CommandProcessing::replconf(lower_str_params_decoded(extra_params), clientfd);
    }
    else if (cmd == "psync"){
        return CommandProcessing::psync(lower_str_params_decoded(extra_params));
    }
    return "-ERR\r\n";
};

void SocketManagement::handle_connection(const int& clientfd){
    bool is_queue_active = false;
    std::vector<std::string> cmds_to_exec;
    while (1) {
        char buffer[256];  
        if (recv(clientfd, &buffer, sizeof(buffer), 0) <= 0) {
            close(clientfd);
            break;
        }

        std::string data(buffer);

        auto command_elts = CommandProcessing::get_command_array_from_rawdata(data);
        std::string cmd = command_elts.first;
        std::vector<DecodedResult*> extra_params = command_elts.second;

        if (is_queue_active){
            if (cmd == "exec"){
                if (cmds_to_exec.size() == 0){
                    is_queue_active = false;
                    CommandProcessing::send_data("*0\r\n", clientfd);
                }
                else {
                    std::string resp_array_str = "*" + std::to_string(cmds_to_exec.size()) + "\r\n";
                    for (auto cmd_data: cmds_to_exec){
                        auto command_and_params = CommandProcessing::get_command_array_from_rawdata(cmd_data);
                        resp_array_str += run_command(command_and_params.first, command_and_params.second, cmd_data, clientfd);
                    }
                    CommandProcessing::send_data(resp_array_str, clientfd);
                    is_queue_active = false;
                    cmds_to_exec.clear();            
                }        
            }
            else if (cmd == "discard") {
                is_queue_active = false;
                cmds_to_exec.clear();            
                CommandProcessing::send_data(parse_encode_simple_string("OK"), clientfd);
            }
            else {
                cmds_to_exec.push_back(data);
                send(clientfd, "+QUEUED\r\n", 9, 0);
            }
            continue;
        }
        else if (cmd == "multi"){
            is_queue_active = true;
            CommandProcessing::send_data(parse_encode_simple_string("OK"), clientfd);
        }
        else if (cmd == "exec"){
            std::string err_msg = "EXEC without MULTI";
            CommandProcessing::send_data(parse_encode_error_msg(err_msg), clientfd);
        }
        else if (cmd == "discard") {
            std::string err_msg = "DISCARD without MULTI";
            CommandProcessing::send_data(parse_encode_error_msg(err_msg), clientfd);        
        }
        else {
            std::string resp = run_command(cmd, extra_params, data, clientfd);
            CommandProcessing::send_data(resp, clientfd);
            if (cmd == "psync")
                CommandProcessing::process_file_datas(clientfd);

        }
    }
}


SocketManagement::SocketManagement(short family, int type, std::map<std::string, std::string> extra) {
    server_fd = socket(family, type, 0);
    socket_family = family;
    socket_type = type;
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

int SocketManagement::send_receive_msg_by_command(std::string tosend, std::string toreceive){
    if (send(server_fd, tosend.c_str(), tosend.length(), 0) <= 0){
        std::cout << "Send "+ tosend + " handshake failed";
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


void SocketManagement::handshake_and_check_incoming_master_connections(int port){
    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cout << "Connect to master failed";
    }
    std::vector<const Encoder*> ping = {&BulkStringEncoder("PING")};
    if(send_receive_msg_by_command(parse_encode_array(ping), "PONG") < 0)
        std::cout << "PING failed";

    std::vector<const Encoder*> replconf_msg1 = {replconfEnc, portListeningEnc, &BulkStringEncoder(std::to_string(port))}; 
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg1), "OK") < 0)
        std::cout << "REPLCONF failed";
    
    std::vector<const Encoder*> replconf_msg2 = {replconfEnc, capaEnc, psync2Enc};
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg2), "OK") < 0)
        std::cout << "REPLCONF failed";

    std::vector<const Encoder*> psync_msg = {psyncEnc, questionMarkEnc, minusOneEnc};
    std::string tosend = parse_encode_array(psync_msg);
    if (send(server_fd, tosend.c_str(), tosend.length(), 0) < 0){
        std::cout << "Send "+ tosend + " handshake failed";
    }

    const int SIZE = 256;
    char buffer[SIZE];  
    int bytes_received = recv(server_fd, &buffer, sizeof(buffer), 0);  
    if (bytes_received <= 0) {
        close(server_fd);
        std::cout << "PSYNC failed";
    }
    int p = 1;
    while (p < bytes_received && ((unsigned char)buffer[p-1] != 0x0d || (unsigned char)buffer[p] != 0x0a)){
        p += 1;
    }
    std::pair<int, std::vector<unsigned char>> file_datas;
    if (p < bytes_received - 1){
        p++;
    }
    else {
        p = 0;
        bytes_received = recv(server_fd, &buffer, sizeof(buffer), 0);
    }
    file_datas = read_file_sent(buffer, SIZE, p);
    retrieve_commands_from_master(bytes_received, buffer, SIZE, p);
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

void SocketManagement::check_incoming_clients_connections(){
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";
  while (1){
      int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len); 
      std::cout << "Client connected \n";
      std::thread connection([this](int clientfd){handle_connection(clientfd);}, client_fd);
      connection.detach();
  }
  close(server_fd);
}

void SocketManagement::process_command(std::string cmd, std::vector<std::string> extra_params) {
    if (cmd == "set"){
        CommandProcessing::set_without_send(extra_params);
    }
    else if (cmd == "replconf"){
        CommandProcessing::replconf(extra_params, this->server_fd);
    }
}

void SocketManagement::retrieve_commands_from_master(int bytes_receive, char* buffe, const int size, int p) {
    char buffer[size];
    for (int i = 0; i < size; i++)
        buffer[i] = buffe[i];
    int bytes_received = bytes_receive;
    int pos = p;
    while (bytes_received > 0){
        std::string data(buffer + pos);
        ArrayDecodeResult arr_resp;
        while (pos < bytes_received){
            arr_resp = parse_decode_array(data);
            auto command = arr_resp.asArray();
            pos += arr_resp.getCharEndIndex();
            std::string cmd = command[0].asString();
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
            std::vector<std::string> array_cmd;
            for (int i = 1; i < command.size(); i++){
                std::string param_str = command[i].asString();
                std::transform(param_str.begin(), param_str.end(), param_str.begin(), ::tolower);
                array_cmd.push_back(param_str);
            }
            if (arr_resp.getCharEndIndex().second < data.size())
                data = data.substr(arr_resp.getCharEndIndex());
            std::cout << "bytes " << bytes_received << "   " << data;
            GlobalDatas::cmdsOffset.set(arr_resp.getCharEndIndex());
            process_command(cmd, array_cmd);
        }
        pos = 0;
        std::memset(buffer, 0, size);
        bytes_received = recv(server_fd, &buffer, size, 0);
    }
};


