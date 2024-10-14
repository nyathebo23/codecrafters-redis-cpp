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

void SocketManagement::execute_command(std::string buffer_data, const int& clientfd) {
    
};

void SocketManagement::handle_connection(const int& clientfd){
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
    for (int i = 0; i < command.size(); i++)
        std::cout << std::any_cast<std::string>(command[i]);
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



void SocketManagement::check_incoming_clients_connections(const int& masterfd){
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
 
  if (masterfd > 0){
      std::thread connection([this, &masterfd](){handle_connection(masterfd);});
      connection.detach();

  }
  std::cout << "Waiting for a client to connect...\n";
  while (1){
      int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len); 
      std::cout << "Client connected " +std::to_string(client_fd)+  "\n";
      std::thread connection([this](int clientfd){handle_connection(clientfd);}, client_fd);
      connection.detach();
  }
  close(server_fd);
}


