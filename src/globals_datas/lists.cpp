#include "lists.h"
#include <algorithm> 

long Lists::append_right(std::string list_key, std::vector<std::string> values) {
    if (this->lists_map.count(list_key) == 0) {
        std::deque list(values.begin(), values.end());
        this->lists_map[list_key] = list;
    }
    else {
        for (std::string val: values) {
            this->lists_map[list_key].push_back(val);
        }
    } 
    return this->lists_map[list_key].size();
};

long Lists::append_left(std::string list_key, std::vector<std::string> values) {
    if (this->lists_map.count(list_key) == 0) {
        std::reverse(values.begin(), values.end());
        std::deque list(values.begin(), values.end());
        this->lists_map[list_key] = list;
    }
    else {
        for (std::string val: values) {
            this->lists_map[list_key].push_front(val);
        }    
    }
    return this->lists_map[list_key].size();
};

std::vector<std::string> Lists::left_range(std::string list_key, long start_ind, long end_ind) {
    if (this->lists_map.count(list_key) == 0) {
        return std::vector<std::string>();
    }
    std::deque<std::string> list = this->lists_map[list_key];
    long list_size = list.size();
    std::vector<std::string> result;
    long start = start_ind < 0 ? list_size + start_ind : start_ind;
    long end = end_ind < 0 ? list_size + end_ind : end_ind;
    start = start < 0 ? 0: start;
    end = end >= list_size ? list_size - 1 : end;

    if (start > end) {
        return std::vector<std::string>();
    }

    auto it_begin = list.begin() + start; 
    auto it_end = list.begin() + end + 1;
    std::copy(it_begin, it_end, std::back_inserter(result));
    return result;
};

long Lists::get_size(std::string list_key) {
    if (this->lists_map.count(list_key) == 0) {
        return 0;
    }
    return this->lists_map[list_key].size();
};

std::optional<std::string> Lists::left_pop(std::string list_key) {
    if (this->lists_map.count(list_key) == 0 || this->lists_map[list_key].size() == 0) {
        return std::nullopt;
    }
    std::deque<std::string>& list_datas = this->lists_map[list_key];
    std::string element = list_datas.front();
    list_datas.pop_front();
    return element;
};

std::vector<std::string> Lists::left_pop_list(std::string list_key, std::size_t count) {
    if (this->lists_map.count(list_key) == 0 || this->lists_map[list_key].size() == 0) {
        return std::vector<std::string>();
    }
    std::deque<std::string>& list_datas = this->lists_map[list_key];
    if (list_datas.size() <= count ) {
        std::vector<std::string> result = std::vector<std::string>(list_datas.begin(), list_datas.end());
        list_datas.clear();
    }
    std::vector<std::string> result;
    std::copy(list_datas.begin(), list_datas.begin() + count, std::back_inserter(result));
    for (size_t i = 0; i < count; i++)
    {
        list_datas.pop_front();
    }
    return result;
};

bool Lists::exist(std::string list_key) {
    return this->lists_map.count(list_key) != 0;
}