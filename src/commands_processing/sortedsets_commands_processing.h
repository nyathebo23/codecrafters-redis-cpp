#ifndef SORTEDSETS_COMMANDS_PROCESSING_H
#define SORTEDSETS_COMMANDS_PROCESSING_H

#include <string>
#include <vector>

class SortedSetsCommandsProcessing {

    public:
        static std::string zadd(const std::vector<std::string>& extras);
        static std::string zrank(const std::vector<std::string>& extras);
        static std::string zrange(const std::vector<std::string>& extras);
        static std::string zcard(const std::vector<std::string>& extras);
        static std::string zscore(const std::vector<std::string>& extras);
        static std::string zrem(const std::vector<std::string>& extras);

};

#endif