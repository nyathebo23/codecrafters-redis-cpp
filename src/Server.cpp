#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <algorithm>
#include "utils/encode/small_aggregate_parser_enc.h"
#include "utils/decode/small_aggregate_parser_dec.h"
#include "utils/encode/simple_data_parser_enc.h"
#include "utils/decode/array_parser_dec.h"
#include <map>

std::map<std::any, std::any> dict_data;

void handle_connection(int clientfd){
  while (1) {
    char buffer[128];    
    if (recv(clientfd, &buffer, sizeof(buffer), 0) <= 0) {
      close(clientfd);
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
              dict_data[vals[1]] = vals[2];
              res = parse_encode_simple_string("OK");
            }
        }
        else if (cmd == "get"){
            if (vals.size() == 2){
               res = parse_encode_bulk_string(dict_data[vals[1]]);
            }
        }
        if (!res.empty())
            send(clientfd, res.c_str(), res.length(), 0);
    }
  }
}


int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";

  while (1){
      int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len); 

      std::cout << "Client connected\n";

      std::thread connection(handle_connection, client_fd);
      connection.detach();
  }

  close(server_fd);

  return 0;
}
