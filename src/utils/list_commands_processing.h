#ifndef LIST_COMMANDS_PROCESSING_H
#define LIST_COMMANDS_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include "decode/decoders.h"

class ListCommandsProcessing {

    protected:
        static void check_blpop_exec(double delay, std::string list_key, int clientfd);

    public:
        static void rpush(std::vector<std::string> extras, int clientfd);
        static std::string lrange(std::vector<std::string> extras);
        static void lpush(std::vector<std::string> extras, int clientfd);
        static std::string llen(std::vector<std::string> extras);
        static std::string lpop(std::vector<std::string> extras);
        static void blpop(std::vector<std::string> extras, int clientfd);
        static void try_run_waiting_lpop(std::string list_key);

};

#endif