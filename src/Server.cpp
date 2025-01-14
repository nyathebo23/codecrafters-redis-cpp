#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <map>
#include "utils/socket_management.h"
#include "utils/global_datas.h"

int main(int argc, char **argv) {
  std::map<std::string, std::string> args_map;
  for (int i = 1; i < argc-1; i+=2){
     args_map[std::string(argv[i]).substr(2)] = std::string(argv[i+1]);
  }

  SocketManagement socket_management(AF_INET, SOCK_STREAM, args_map);

  std::map<std::string, std::string> args_map_master;
  if (args_map.count("replicaof") != 0){
      args_map_master["host"] = args_map["replicaof"].substr(0, args_map["replicaof"].find_first_of(" "));
      args_map_master["port"] = args_map["replicaof"].substr(args_map["replicaof"].find_last_of(" ")+1);
      GlobalDatas::isMaster = false;
  }


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

  SocketManagement master_socket_management(AF_INET, SOCK_STREAM, args_map_master);
  if (args_map.count("replicaof") != 0){
      std::thread connection([&socket_management, &master_socket_management]() {
        master_socket_management.handshake_and_check_incoming_master_connections(ntohs(socket_management.get_server_addr().sin_port));
      });
      connection.detach();
      //master_socket_management.check_incoming_master_connections();
  }
    socket_management.check_incoming_clients_connections();



  return 0;
}
