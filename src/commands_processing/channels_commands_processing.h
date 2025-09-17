#ifndef CHANNELS_COMMANDS_PROCESSING_H
#define CHANNELS_COMMANDS_PROCESSING_H

#include <string>
#include <map>
#include <vector>

class ChannelsCommandsProcessing {
    private:
        static std::string unallowed_command_error(std::string cmdname) {
            return "Can't execute '" + cmdname + "': only (P|S)SUBSCRIBE / (P|S)UNSUBSCRIBE / PING / QUIT / RESET are allowed in this context";
        }
    public:
        static void subsribe(const std::vector<std::string>& extras, int clientfd);
        static void unsubscribe(const std::vector<std::string>& extras, int clientfd);
        static void publish(const std::vector<std::string>& extras, int clientfd);
        static void ping(const std::vector<std::string>& extras, int clientfd);
        static void enter_subscribe_mode(int clientfd);
};

#endif