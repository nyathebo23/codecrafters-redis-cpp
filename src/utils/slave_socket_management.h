#ifndef SLAVE_SOCKET_MANAGEMENT_H
#define SLAVE_SOCKET_MANAGEMENT_H

#include <string>
#include "socket_management.h"

class SlaveSocketManagement : public SocketManagement {
    private:
        void process_command(std::string data);

    public:
        SlaveSocketManagement(short family, int type, std::map<std::string, std::string> extra);
    
        void check_incoming_master_connections(const int &masterfd);

        void retrieve_commands_from_master(const int& serverfd);

};

#endif