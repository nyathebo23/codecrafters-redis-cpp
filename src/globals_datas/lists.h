#ifndef LISTS_DATAS_H
#define LISTS_DATAS_H

#include <string>
#include <map>
#include <vector>
#include <deque>
#include <optional>


class Lists {
    public:
        //void set(std::string list_key, std::vector<std::string> values);
        bool exist(std::string list_key);
        int append_right(std::string list_key, std::vector<std::string> values);
        int append_left(std::string list_key, std::vector<std::string> values);
        int get_size(std::string list_key);
        std::vector<std::string> left_range(std::string list_key, int start_ind, int end_ind);
        std::optional<std::string> left_pop(std::string list_key);
        std::vector<std::string> left_pop_list(std::string list_key, std::size_t count);

    private:
        std::map<std::string, std::deque<std::string>> lists_map;

};

#endif