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
        std::map<std::string, std::string> dict_data;
        //std::vector<int> replicas_fd;

        std::pair<std::string, std::vector<std::string>> get_command_array_from_rawdata(std::string data);

    public:
        void handle_connection(const int& clientfd);

        short socket_family;

        int socket_type;

        void newsocket();

        CommandProcessing command_processing;

        virtual void execute_command(std::string buffer_data, int& clientfd);

        SocketManagement(short family, int type, std::map<std::string, std::string> extra);

        int get_server_fd() const;

        struct sockaddr_in get_server_addr() const;
        //sockaddr_in get_addr_from_params_datas(std::string master_raw_data);

        int socket_bind();

        int socket_listen(int connection_backlog);

        void check_incoming_clients_connections(const int& fd);

};


#endif