#ifndef SLAVE_SOCKET_MANAGEMENT_H
#define SLAVE_SOCKET_MANAGEMENT_H

#include <string>
#include "socket_management.h"

class SlaveSocketManagement : public SocketManagement {
    private:
        void process_command(std::string data, const int& fd);

    public:
        SlaveSocketManagement(short family, int type, std::map<std::string, std::string> extra);

        void execute_command(std::string buffer_data, const int& clientfd) override;

        void check_incoming_master_connections(const int &masterfd);

        void retrieve_commands_from_master(const int& serverfd);

};

#endif