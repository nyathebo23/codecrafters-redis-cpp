#ifndef SOCKET_MANAGEMENT_H
#define SOCKET_MANAGEMENT_H

#include <string>
#include <thread>
#include <map>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "command_processing.h"

class SocketManagement {
    protected:
        int server_fd;

        struct sockaddr_in server_addr;
        
        std::map<std::string, std::string> extra_args;

        std::vector<int> replicas_fd;

        std::pair<std::string, std::vector<std::string>> get_command_array_from_rawdata(std::string data);

        int send_receive_msg_by_command(std::string tosend, std::string toreceive);

        void process_command(std::string data);

    public:
        void handle_connection(const int& clientfd);

        short socket_family;

        int socket_type;

        SocketManagement(short family, int type, std::map<std::string, std::string> extra);

        int get_server_fd() const;

        struct sockaddr_in get_server_addr() const;

        int socket_bind();

        int socket_listen(int connection_backlog);

        void check_incoming_clients_connections();

        void send_handshake_to_master(int port);

        void check_incoming_master_connections(const int& fd);

        void retrieve_commands_from_master(const int& serverfd)
};

#endif