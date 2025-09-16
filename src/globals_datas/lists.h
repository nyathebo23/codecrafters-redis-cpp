#ifndef LISTS_DATAS_H
#define LISTS_DATAS_H

#include <string>
#include <map>
#include <vector>
#include <deque>
#include <optional>


class Lists {
    public:
        bool exist(std::string list_key);
        long append_right(std::string list_key, std::vector<std::string> values);
        long append_left(std::string list_key, std::vector<std::string> values);
        long get_size(std::string list_key);
        
        std::vector<std::string> left_range(std::string list_key, long start_ind, long end_ind);
        std::optional<std::string> left_pop(std::string list_key);
        std::vector<std::string> left_pop_list(std::string list_key, std::size_t count);
        bool isClientWaitingBLPOP(std::string list_key);
        void addOnWaitingBLPOPList(std::string list_key, int clientfd);
        bool checkAndDeleteClientWaiting(std::string list_key, int clientfd);
        void deleteListKeysByClientFD(int clientfd);
        int getFirstClient(std::string list_key);

    private:
        std::map<std::string, std::vector<int>> mapKeysClientsBLPOP;
        std::map<int, std::vector<std::string>> mapClientsBLPOPKeys;
        std::map<std::string, std::deque<std::string>> lists_map;
        void deleteListKeyByClientFD(std::string list_key, int clientfd);

};

#endif