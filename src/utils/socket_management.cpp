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
#include "command_processing.h"

void SocketManagement::execute_command(std::string buffer_data, int clientfd) {
    auto command_elts = this->get_command_array_from_rawdata(buffer_data);
    std::string cmd = command_elts.first;
    std::vector<std::string> extra_params = command_elts.second;
    if (cmd == "echo"){
        command_processing.echo(extra_params, clientfd);
    }
    else if (cmd == "ping"){
        command_processing.ping(clientfd);
    }
    else if (cmd == "set"){
        for (int replica_fd: this->replicas_fd) {
            if (send(replica_fd, buffer_data.c_str(), buffer_data.length(), 0) <= 0)
                std::cout <<  "replica send msg failed";                
        }
        command_processing.set(extra_params, clientfd);
    }
    else if (cmd == "get"){
        command_processing.get(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "config"){
        command_processing.config(extra_params, clientfd, extra_args);
    }
    else if (cmd == "keys"){
        command_processing.keys(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "info"){
        command_processing.info(extra_params, clientfd, "master");
    }
    else if (cmd == "replconf"){
        command_processing.replconf(extra_params, clientfd);
    }
    // else if (cmd == "psync"){
    //     command_processing.psync(extra_params, clientfd, replicas_fd);
    // }
    
};

void SocketManagement::handle_connection(int clientfd){
    while (1) {
        char buffer[128];    
        if (recv(clientfd, &buffer, sizeof(buffer), 0) <= 0) {
            close(clientfd);
            break;
        }
        std::string data(buffer);
        execute_command(data, clientfd);
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

void SocketManagement::newsocket() {
    this->server_fd = socket(this->socket_family, this->socket_type, 0);
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
      std::cout << "Client connected\n";
      std::thread connection([this](int clientfd){handle_connection(clientfd);}, client_fd);
      connection.detach();
  }
  close(server_fd);
}


