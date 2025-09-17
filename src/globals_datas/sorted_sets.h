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
        long zadd(std::string setName, double score, std::string member);
        std::optional<long> zrank(std::string setName, std::string member);
        std::vector<std::string> zrange(std::string setName, long start, long end);
        std::list<SortedSetElement> zall(std::string setName);
        long zcard(std::string setName);
        std::string zscore(std::string setName, std::string member);
        long zrem(std::string setName, std::string member);

};

#endif