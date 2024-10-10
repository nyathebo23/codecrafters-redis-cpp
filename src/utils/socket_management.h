#ifndef SOCKETMANAGEMENT_H
#define SOCKETMANAGEMENT_H

#include <string>
#include <thread>
#include <map>
#include <arpa/inet.h>

class SocketManagement {
    private:
        int server_fd;
        int client_fd;
        struct sockaddr_in server_addr;
        std::map<std::string, std::string> extra_args;
        std::map<std::string, std::string> dict_data;

        void erase_key(const std::string& key);

        void execute_after_delay(int delay, const std::string& key);

        sockaddr_in get_addr_from_params_datas(std::string master_raw_data);

        void handle_connection();

    public:
        SocketManagement(short family, int type, std::map<std::string, std::string> extra);
        int get_server_fd() const;

        int socket_bind();

        int socket_listen(int connection_backlog);

        int send_message_to_server(sockaddr_in dest_address, std::string msg);

        void check_incoming_clients_connections();

};

#endif