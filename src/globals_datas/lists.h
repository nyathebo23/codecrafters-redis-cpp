#ifndef LISTS_DATAS_H
#define LISTS_DATAS_H

#include <string>
#include <map>
#include <vector>
#include <deque>
#include <optional>


class Lists {
    public:
        bool exist(const std::string& list_key);
        long append_right(const std::string& list_key, const std::vector<std::string>& values);
        long append_left(const std::string& list_key, std::vector<std::string> values);
        long get_size(const std::string& list_key);
        
        std::vector<std::string> left_range(const std::string& list_key, const long& start_ind, const long& end_ind);
        std::optional<std::string> left_pop(const std::string& list_key);
        std::vector<std::string> left_pop_list(const std::string& list_key, const std::size_t& count);
        bool is_client_waiting_blpop(const std::string& list_key);
        void add_on_waiting_blpop_list(const std::string& list_key, int clientfd);
        bool check_and_delete_client_waiting(const std::string& list_key, int clientfd);
        void delete_list_keys_by_client_fd(int clientfd);
        int get_first_client(const std::string& list_key);

    private:
        std::map<std::string, std::vector<int>> mapKeysClientsBLPOP;
        std::map<int, std::vector<std::string>> mapClientsBLPOPKeys;
        std::map<std::string, std::deque<std::string>> lists_map;
        void delete_list_key_by_client_fd(const std::string& list_key, const int& clientfd);

};

#endif