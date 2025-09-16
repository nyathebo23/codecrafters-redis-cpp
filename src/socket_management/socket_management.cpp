#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "../utils/encode/array_parser_enc.h"
#include "../utils/encode/simple_data_parser_enc.h"
#include "../utils/encode/small_aggregate_parser_enc.h"
#include "../utils/decode/array_parser_dec.h"
#include "../utils/decode/small_aggregate_parser_dec.h"
#include "../utils/process_rdbfile_datas.h"
#include "../utils/decode/simple_data_parser_dec.h"
#include "socket_management.h"
#include "../commands_processing/command_processing.h"
#include "../commands_processing/channels_commands_processing.h"
#include "../commands_processing/stream_commands_processing.h"
#include "../commands_processing/list_commands_processing.h"
#include "../globals_datas/global_datas.h"
#include "../utils/resp_constants.h"
#include <iomanip>


void SocketManagement::handle_connection(const int& clientfd){

    while (1) {
        char buffer[1024] = {0};  
        int bytesReceived = recv(clientfd, &buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            close(clientfd);
            break;
        }
        buffer[bytesReceived] = '\0';
        std::string data(buffer);

        auto command = CommandProcessing::get_command_array_from_rawdata(data);
        if (command.error.has_value()){
            CommandProcessing::send_data(parse_encode_error_msg(command.error.value()), clientfd);
            continue;
        }
        std::string cmd = command.name;
        if (cmd == "subscribe") {
            ChannelsCommandsProcessing::subsribe(command.args, clientfd);
            ChannelsCommandsProcessing::enter_subscribe_mode(clientfd);
        }
        else if (cmd == "unsubscribe") {
            ChannelsCommandsProcessing::unsubscribe(command.args, clientfd);
        }
        else if (cmd == "publish") {
            ChannelsCommandsProcessing::publish(command.args, clientfd);
        }
        else if (cmd == "blpop") {
            ListCommandsProcessing::blpop(command.args, clientfd);
        }
        else if (cmd == "rpush") {
            ListCommandsProcessing::rpush(command.args, clientfd);
        }
        else if (cmd == "lpush") {
            ListCommandsProcessing::lpush(command.args, clientfd);
        }
        else if (cmd == "multi"){
            CommandProcessing::multi(clientfd, extra_args);
        }
        else if (cmd == "exec"){
            std::string err_msg = "EXEC without MULTI";
            CommandProcessing::send_data(parse_encode_error_msg(err_msg), clientfd);
        }
        else if (cmd == "discard") {
            std::string err_msg = "DISCARD without MULTI";
            CommandProcessing::send_data(parse_encode_error_msg(err_msg), clientfd);        
        }
        else if (cmd == "psync"){
            CommandProcessing::psync(command.args, clientfd);
        }
        else {
            std::string resp = CommandProcessing::get_command_response(cmd, command.args, data, clientfd, extra_args);
            CommandProcessing::send_data(resp, clientfd);
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
    StringDecodeResult* data_decoded = parse_decode_simple_string(data);
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

    if(send_receive_msg_by_command(pingAsListEnc, "PONG") < 0)
        std::cout << "PING failed";

    std::string replconfMsg1Enc = replconfMsg1PartEnc + parse_encode_bulk_string(std::to_string(port));
    if(send_receive_msg_by_command(replconfMsg1Enc, "OK") < 0)
        std::cout << "REPLCONF failed";
    
    if(send_receive_msg_by_command(replconfMsg2Enc, "OK") < 0)
        std::cout << "REPLCONF failed";

    if (send(server_fd, psyncMsgEnc.c_str(), psyncMsgEnc.length(), 0) < 0){
        std::cout << "Send "+ psyncMsgEnc + " handshake failed";
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
        ArrayDecodeResult arr_resp = ArrayDecodeResult(std::vector<DecodedResultPtr>(), 0);
        while (pos < bytes_received){
            arr_resp = parse_decode_array(data);
            auto command = arr_resp.asArray();
            pos += arr_resp.getCharEndIndex();
            std::string cmd = command[0]->asString();
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
            std::vector<std::string> array_cmd;
            for (int i = 1; i < command.size(); i++){
                std::string param_str = command[i]->asString();
                std::transform(param_str.begin(), param_str.end(), param_str.begin(), ::tolower);
                array_cmd.push_back(param_str);
            }
            if (arr_resp.getCharEndIndex() < data.size())
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


