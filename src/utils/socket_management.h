#ifndef SOCKET_MANAGEMENT_H
#define SOCKET_MANAGEMENT_H

#include <string>
#include <thread>
#include <map>
#include <vector>
#include "command_processing.h"

class SocketManagement {

    protected:
        int server_fd;

        struct sockaddr_in server_addr;
        
        std::map<std::string, std::string> extra_args;

        int send_receive_msg_by_command(std::string tosend, std::string toreceive);

        void process_command(std::string, std::vector<std::string>);
        
        std::vector<std::string> lower_str_params_decoded(std::vector<DecodedResult*> params);
    
    public:
        void handle_connection(const int& clientfd);

        std::string run_command(std::string cmd, std::vector<DecodedResult*> extra_params, std::string data, int clientfd);

        short socket_family;

        int socket_type;

        SocketManagement(short family, int type, std::map<std::string, std::string> extra);

        int get_server_fd() const;

        struct sockaddr_in get_server_addr() const;

        int socket_bind();

        int socket_listen(int connection_backlog);

        void check_incoming_clients_connections();

        void handshake_and_check_incoming_master_connections(int port);

        void retrieve_commands_from_master(int bytes_received, char* buffer, const int size, int pos);
};

#endif