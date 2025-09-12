#ifndef LIST_COMMANDS_PROCESSING_H
#define LIST_COMMANDS_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include <any>


class ListCommandsProcessing {

    public:
        static std::string rpush(std::vector<std::string> extras);
        static std::string lrange(std::vector<std::any> extras);
        static std::string lpush(std::vector<std::string> extras);
        static std::string llen(std::vector<std::string> extras);
        static std::string lpop(std::vector<std::any> extras);
        static std::string blpop(std::vector<std::any> extras);

};

#endif