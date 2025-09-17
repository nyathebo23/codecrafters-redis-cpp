#include "sorted_sets.h"
#include <algorithm> 
#include <iterator>
#include <sstream>
#include <iomanip>

bool SortedSetElement::operator<(const SortedSetElement& other) const {
    return score < other.score || (score == other.score && member < other.member );
}

long SortedSets::zadd(std::string setName, double score, std::string member) {
    auto &sortedSet = this->setsMap[setName];
    for (SortedSetElement &item: sortedSet) {
        if (item.member == member) {
            item.score = score;
            sortedSet.sort();
            return 0;
        }
    }
    SortedSetElement element = {
        .member = member,
        .score = score
    };
    auto iter = std::lower_bound(sortedSet.begin(), sortedSet.end(), element);
    sortedSet.insert(iter, element);
    return 1;
}

std::optional<long> SortedSets::zrank(std::string setName, std::string member) {
    if (this->setsMap.count(setName) == 0) return std::nullopt;
    auto sortedSet = this->setsMap[setName];
    auto iter = sortedSet.begin();
    while (iter != sortedSet.end() && (*iter).member != member)
    {
        ++iter;
    }
    if (iter == sortedSet.end()) return std::nullopt;;
    return std::distance(sortedSet.begin(), iter);
};

std::vector<std::string> SortedSets::zrange(std::string setName, long start, long end) {
    auto sortedSet = this->setsMap[setName];
    long listsize = sortedSet.size();
    long startind = start < 0 ? listsize + start : start;
    long endind = end < 0 ? listsize + end : end;
    startind = startind < 0 ? 0: startind;
    endind = endind >= listsize ? listsize - 1 : endind;

    if (startind > endind) {
        return std::vector<std::string>();
    }
    std::vector<std::string> result;

    auto it_begin = sortedSet.begin();
    std::advance(it_begin, startind);
    auto it_end = sortedSet.begin();
    std::advance(it_end, endind + 1);

    for (auto it = it_begin; it != it_end; ++it) {
        result.push_back((*it).member);
    }    
    return result;
}

std::list<SortedSetElement> SortedSets::zall(std::string setName) {
    return this->setsMap[setName];   
}

long SortedSets::zcard(std::string setName) {
    auto sortedSet = this->setsMap[setName];
    return sortedSet.size();
}

std::string SortedSets::zscore(std::string setName, std::string member) {
    auto sortedSet = this->setsMap[setName];
    for (SortedSetElement item: sortedSet) {
        if (item.member == member) {
            std::ostringstream score_ostream;
            score_ostream << std::setprecision(17) << item.score;
            return score_ostream.str();
        }
    } 
    return ""; 
}

long SortedSets::zrem(std::string setName, std::string member) {
    if (this->setsMap.count(setName) == 0) return 0;
    auto &sortedSet = this->setsMap[setName];
    auto iter = sortedSet.begin();
    while (iter != sortedSet.end() && (*iter).member != member)
    {
        ++iter;
    }
    if (iter == sortedSet.end())
        return 0;
    sortedSet.erase(iter);
    return 1;
};