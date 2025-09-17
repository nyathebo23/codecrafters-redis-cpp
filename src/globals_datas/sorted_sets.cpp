#include "sorted_sets.h"
#include <algorithm> 
#include <iterator>
#include <sstream>
#include <iomanip>

bool SortedSetElement::operator<(const SortedSetElement& other) const {
    return score < other.score || (score == other.score && member < other.member );
}

long SortedSets::zadd(const std::string& setName, const double& score, const std::string& member) {
    auto &sorted_set = this->setsMap[setName];
    for (SortedSetElement &item: sorted_set) {
        if (item.member == member) {
            item.score = score;
            sorted_set.sort();
            return 0;
        }
    }
    SortedSetElement element = {
        .member = member,
        .score = score
    };
    auto iter = std::lower_bound(sorted_set.begin(), sorted_set.end(), element);
    sorted_set.insert(iter, element);
    return 1;
}

std::optional<long> SortedSets::zrank(const std::string& setName, const std::string& member) {
    if (this->setsMap.count(setName) == 0) return std::nullopt;
    auto sorted_set = this->setsMap[setName];
    auto iter = sorted_set.begin();
    while (iter != sorted_set.end() && (*iter).member != member)
    {
        ++iter;
    }
    if (iter == sorted_set.end()) return std::nullopt;;
    return std::distance(sorted_set.begin(), iter);
};

std::vector<std::string> SortedSets::zrange(const std::string& setName, const long& start, const long& end) {
    auto sorted_set = this->setsMap[setName];
    long listsize = sorted_set.size();
    long startind = start < 0 ? listsize + start : start;
    long endind = end < 0 ? listsize + end : end;
    startind = startind < 0 ? 0: startind;
    endind = endind >= listsize ? listsize - 1 : endind;

    if (startind > endind) {
        return std::vector<std::string>();
    }
    std::vector<std::string> result;

    auto it_begin = sorted_set.begin();
    std::advance(it_begin, startind);
    auto it_end = sorted_set.begin();
    std::advance(it_end, endind + 1);

    for (auto it = it_begin; it != it_end; ++it) {
        result.push_back((*it).member);
    }    
    return result;
}

std::list<SortedSetElement> SortedSets::zall(const std::string& setName) {
    return this->setsMap[setName];   
}

long SortedSets::zcard(const std::string& setName) {
    auto sorted_set = this->setsMap[setName];
    return sorted_set.size();
}

std::string SortedSets::zscore(const std::string& setName, const std::string& member) {
    auto sorted_set = this->setsMap[setName];
    for (SortedSetElement item: sorted_set) {
        if (item.member == member) {
            std::ostringstream score_ostream;
            score_ostream << std::setprecision(17) << item.score;
            return score_ostream.str();
        }
    } 
    return ""; 
}

long SortedSets::zrem(const std::string& setName, const std::string& member) {
    if (this->setsMap.count(setName) == 0) return 0;
    auto &sorted_set = this->setsMap[setName];
    auto iter = sorted_set.begin();
    while (iter != sorted_set.end() && (*iter).member != member)
    {
        ++iter;
    }
    if (iter == sorted_set.end())
        return 0;
    sorted_set.erase(iter);
    return 1;
};