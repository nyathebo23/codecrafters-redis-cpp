#include <iostream>
#include <string>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "utils/socket_management.h"


int main(int argc, char **argv) {
  std::map<std::string, std::string> args_map;
  for (int i = 1; i < argc-1; i+=2){
     args_map[std::string(argv[i])] = std::string(argv[i+1]);
  }
  std::map<std::string, std::string> args_map2;

  SocketManagement master_server_socket(AF_INET, SOCK_STREAM, args_map2);

  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  if (master_server_socket.get_server_fd() < 0) {
   std::cerr << "Failed to create master server socket\n";
   return 1;
  }
  // // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(master_server_socket.get_server_fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  if (master_server_socket.socket_bind() != 0) {
    std::cerr << "Failed master to bind to port " + std::to_string(master_server_socket.get_server_fd()) + " \n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (master_server_socket.socket_listen(connection_backlog) != 0) {
    std::cerr << "listen master failed\n";
    return 1;
  }
  
  master_server_socket.check_incoming_clients_connections();
  
 

  return 0;
}
