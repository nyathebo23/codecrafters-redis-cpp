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
#include "global_datas.h"
#include <iomanip>

void SocketManagement::handle_connection(const int& clientfd){

    while (1) {
        char buffer[256];  
        if (int r = recv(clientfd, &buffer, sizeof(buffer), 0) <= 0) {
            close(clientfd);
            break;
        }
        std::string data(buffer);
        auto command_elts = this->get_command_array_from_rawdata(data);
        std::string cmd = command_elts.first;
        std::vector<std::string> extra_params = command_elts.second;

        if (cmd == "echo"){
            CommandProcessing::echo(extra_params, clientfd);
        }
        else if (cmd == "ping"){
            CommandProcessing::ping(clientfd);
        }
        else if (cmd == "set"){
            if (GlobalDatas::isMaster)
                for (int replica_fd: this->replicas_fd) {
                    if (send(replica_fd, data.c_str(), data.length(), 0) <= 0)
                        std::cout <<  "replica send msg failed";                
                }
            CommandProcessing::set(extra_params, clientfd);
        }
        else if (cmd == "get"){
            CommandProcessing::get(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
        }
        else if (cmd == "config"){
            CommandProcessing::config(extra_params, clientfd, extra_args);
        }
        else if (cmd == "keys"){
            CommandProcessing::keys(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
        }
        else if (cmd == "info"){
            CommandProcessing::info(extra_params, clientfd, GlobalDatas::isMaster ? "master" : "slave");
        }
        else if (cmd == "replconf"){
            CommandProcessing::replconf(extra_params, clientfd);
        }
        else if (cmd == "psync"){
            CommandProcessing::psync(extra_params, clientfd, replicas_fd);
        }
    }
}

std::pair<std::string, std::vector<std::string>> SocketManagement::get_command_array_from_rawdata(std::string data){
    ArrayResp arr_resp = parse_decode_array(data);
    auto arr = std::get<ArrayAndInd>(arr_resp.first);
    auto command = arr.first;
    std::string cmd = std::any_cast<std::string>(command[0]);
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    std::vector<std::string> array_cmd;
    for (int i = 1; i < command.size(); i++){
        std::string param = std::any_cast<std::string>(command[i]);
        std::transform(param.begin(), param.end(), param.begin(), ::tolower);
        array_cmd.push_back(param);
    }
    return std::make_pair(cmd, array_cmd);
};


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
    std::cout << std::endl;
    std::string data(buffer);
    std::string data_decoded = parse_decode_simple_string(data).first;
    if (data_decoded != toreceive){
        std::cout << "Bad message receive to " + tosend + " which is " + data_decoded;
        return -1;
    }
    return 1;
};


void SocketManagement::send_handshake_to_master(int port){
    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cout << "Connect to master failed";
    }
    std::vector<std::any> ping = {std::string("PING")};
    if(send_receive_msg_by_command(parse_encode_array(ping), "PONG") < 0)
        std::cout << "PING failed";

    std::vector<std::any> replconf_msg1 = {std::string("REPLCONF"), std::string("listening-port"), std::to_string(port)}; 
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg1), "OK") < 0)
        std::cout << "REPLCONF failed";
    
    std::vector<std::any> replconf_msg2 = {std::string("REPLCONF"), std::string("capa"), std::string("psync2")};
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg2), "OK") < 0)
        std::cout << "REPLCONF failed";

    std::vector<std::any> psync_msg = {std::string("PSYNC"), std::string("?"), std::string("-1")};
    std::string tosend = parse_encode_array(psync_msg);
    if (send(server_fd, tosend.c_str(), tosend.length(), 0) < 0){
        std::cout << "Send "+ tosend + " handshake failed";
    }
    char buffer[256];  
    int bytes_received = recv(server_fd, &buffer, sizeof(buffer), 0);  
    if (bytes_received <= 0) {
        close(server_fd);
        std::cout << "PSYNC failed";
    }

    int p = 1;
    while (p < bytes_received && ((unsigned char)buffer[p-1] != 0x0d || (unsigned char)buffer[p] != 0x0a)){
        p += 1;
    }
    std::cout << "p " << p << "bytes_received " << bytes_received;
    std::pair<int, std::vector<unsigned char>> file_datas;
    if (p < bytes_received - 1){
        p++;
        file_datas = read_file_sent(buffer, 256, p);
        if (p < bytes_received){
            process_command(std::string(buffer + p));
        }
    }
    else {
        int p = 0;
        bytes_received = recv(server_fd, &buffer, sizeof(buffer), 0);
        file_datas = read_file_sent(buffer, 256, p);
        if (p < bytes_received){
            process_command(std::string(buffer + p));
        }
    }
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
  //GlobalDatas::isRequestFromMaster = false;

  std::cout << "Waiting for a client to connect...\n";
  while (1){
      int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len); 
      std::cout << "Client connected " +std::to_string(client_fd)+  " \n";
      std::thread connection([this](int clientfd){handle_connection(clientfd);}, client_fd);
      connection.detach();
  }
  close(server_fd);

}

void SocketManagement::check_incoming_master_connections(){
    std::thread connection([this](){retrieve_commands_from_master();});
    connection.detach();
}

void SocketManagement::process_command(std::string data) {
    auto command_elts = this->get_command_array_from_rawdata(data);
    std::string cmd = command_elts.first;
    std::vector<std::string> extra_params = command_elts.second;
    if (cmd == "set"){
        CommandProcessing::set_without_send(extra_params);
    }
    else if (cmd == "replconf"){
        CommandProcessing::replconf(extra_params, this->server_fd);
    }
}

void SocketManagement::retrieve_commands_from_master() {
    while (1){
        char buffer[128];    
        if (recv(server_fd, &buffer, sizeof(buffer), 0) <= 0)
            break;
        
        std::string data(buffer);
        GlobalDatas::set_commands_offset(data);
        process_command(data);
    }
};

void SocketManagement::recv_rdb_file(int master_fd) 
{
  char buf[128];
  bool size_determined = false;
  int size = 0;
  while(!size_determined) 
  {
    recv(master_fd, buf, 1, 0);
    std::cout << buf[0] <<"\n";
    if(buf[0] == '$' || buf[0] == '\r') 
      continue;
    else if (buf[0] == '\n')
      size_determined = true;
    else 
      size = size * 10 + (buf[0] - '0');
  }
  std::cout << size << "\n";
  int bytes_recvd = 0;
  while(bytes_recvd < size)
  {
    recv(master_fd, buf, 1, 0);
    std::cout << buf[0];
    bytes_recvd++;
  }
}

