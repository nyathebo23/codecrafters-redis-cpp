#ifndef SLAVE_SOCKET_MANAGEMENT_H
#define SLAVE_SOCKET_MANAGEMENT_H

#include <string>
#include "socket_management.h"

class SlaveSocketManagement : public SocketManagement {

    public:
        SlaveSocketManagement(short family, int type, std::map<std::string, std::string> extra);

        void execute_command(std::string buffer_data, int& clientfd) override;        

};

#endif