#ifndef MASTER_SOCKET_MANAGEMENT_H
#define MASTER_SOCKET_MANAGEMENT_H

#include <string>
#include <vector>
#include "socket_management.h"

class MasterSocketManagement : public SocketManagement {
    private:
        int send_receive_msg_by_command(std::string tosend, std::string toreceive);

    public:
        std::vector<int> replicas_fd;
        int send_handshake_to_master(int port);
 
};

#endif