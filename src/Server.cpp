#include "utils/socket_management.h"


int main(int argc, char **argv) {
  std::map<std::string, std::string> args_map;
  for (int i = 1; i < argc-1; i+=2){
     args_map[std::string(argv[i])] = std::string(argv[i+1]);
  }

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

  return 0;
}
