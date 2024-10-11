#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <map>
#include "utils/socket_management.h"


int main(int argc, char **argv) {
  std::map<std::string, std::string> args_map;
  for (int i = 1; i < argc-1; i+=2){
     args_map[std::string(argv[i]).substr(2)] = std::string(argv[i+1]);
  }

  std::map<std::string, std::string> args_map_master;
  if (args_map.count("replicaof") != 0){
      args_map_master["host"] = args_map["replicaof"].substr(0, args_map["replicaof"].find_first_of(" "));
      args_map_master["port"] = args_map["replicaof"].substr(args_map["replicaof"].find_last_of(" ")+1);
  }
  //std::string dest_port = master_raw_data.substr(master_raw_data.find_first_of(" ")+1);

  SocketManagement master_socket_management(AF_INET, SOCK_STREAM, args_map_master);
  master_socket_management.send_message_to_server(std::string("PING"));

  SocketManagement socket_management(AF_INET, SOCK_STREAM, args_map);


  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;


  if (socket_management.get_server_fd() < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  // // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(socket_management.get_server_fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  if (socket_management.socket_bind() != 0) {
    std::cerr << "Failed to bind to port \n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (socket_management.socket_listen(connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }
  
  socket_management.check_incoming_clients_connections();
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  std::cout << "Waiting for a client to connect...\n";
  while (1){
      int clientfd = accept(socket_management.get_server_fd(), (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len); 
      std::cout << "Client connected\n";
      std::thread connection([&clientfd](){handle_connection(clientfd);});
      connection.detach();
  }
  close(socket_management.get_server_fd());

  return 0;
}
