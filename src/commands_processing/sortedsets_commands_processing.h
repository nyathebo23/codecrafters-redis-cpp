#ifndef SORTEDSETS_COMMANDS_PROCESSING_H
#define SORTEDSETS_COMMANDS_PROCESSING_H

#include <string>
#include <vector>

class SortedSetsCommandsProcessing {

    public:
        static std::string zadd(std::vector<std::string> extras);
        static std::string zrank(std::vector<std::string> extras);
        static std::string zrange(std::vector<std::string> extras);
        static std::string zcard(std::vector<std::string> extras);
        static std::string zscore(std::vector<std::string> extras);
        static std::string zrem(std::vector<std::string> extras);

};

#endif