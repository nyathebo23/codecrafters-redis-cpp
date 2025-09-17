#ifndef SORTED_SETS_H
#define SORTED_SETS_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <optional>

struct SortedSetElement
{
    std::string member;
    double score;

    bool operator<(const SortedSetElement& other) const;
};

class SortedSets {
    private:
        std::map<std::string, std::list<SortedSetElement>> setsMap;

    public:
        long zadd(const std::string& setName, const double& score, const std::string& member);
        std::optional<long> zrank(const std::string& setName, const std::string& member);
        std::vector<std::string> zrange(const std::string& setName, const long& start, const long& end);
        std::list<SortedSetElement> zall(const std::string& setName);
        long zcard(const std::string& setName);
        std::string zscore(const std::string& setName, const std::string& member);
        long zrem(const std::string& setName, const std::string& member);

};

#endif