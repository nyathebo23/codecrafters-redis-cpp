#ifndef LIST_COMMANDS_PROCESSING_H
#define LIST_COMMANDS_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include "../utils/decode/decoders.h"

class ListCommandsProcessing {

    protected:
        static void check_blpop_exec(const double& delay, const std::string& list_key, const int& clientfd);

    public:
        static void rpush(const std::vector<std::string>& extras, const int& clientfd);
        static std::string lrange(const std::vector<std::string>& extras);
        static void lpush(const std::vector<std::string>& extras, const int& clientfd);
        static std::string llen(const std::vector<std::string>& extras);
        static std::string lpop(const std::vector<std::string>& extras);
        static void blpop(const std::vector<std::string>& extras, const int& clientfd);
        static void try_run_waiting_lpop(std::string list_key);

};

#endif