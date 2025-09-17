#include "lists.h"
#include <algorithm> 
#include <iostream>

long Lists::append_right(const std::string& list_key, const std::vector<std::string>& values) {
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

long Lists::append_left(const std::string& list_key, std::vector<std::string> values) {
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

std::vector<std::string> Lists::left_range(const std::string& list_key, const long& start_ind, const long& end_ind) {
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

long Lists::get_size(const std::string& list_key) {
    if (this->lists_map.count(list_key) == 0) {
        return 0;
    }
    return this->lists_map[list_key].size();
};

std::optional<std::string> Lists::left_pop(const std::string& list_key) {
    if (this->lists_map.count(list_key) == 0 || this->lists_map[list_key].size() == 0) {
        return std::nullopt;
    }
    std::deque<std::string>& list_datas = this->lists_map[list_key];
    std::string element = list_datas.front();
    list_datas.pop_front();
    return element;
};

std::vector<std::string> Lists::left_pop_list(const std::string& list_key, const std::size_t& count) {
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

bool Lists::exist(const std::string& list_key) {
    return this->lists_map.count(list_key) != 0;
}

bool Lists::is_client_waiting_blpop(const std::string& list_key) {
    return this->exist(list_key) && this->mapKeysClientsBLPOP[list_key].size() != 0;
};

void Lists::add_on_waiting_blpop_list(const std::string& list_key, int clientfd) {
    this->mapKeysClientsBLPOP[list_key].push_back(clientfd);
    this->mapClientsBLPOPKeys[clientfd].push_back(list_key);
};

void Lists::delete_list_key_by_client_fd(const std::string& list_key, const int& clientfd) {
    std::vector<std::string>& client_keys_list = this->mapClientsBLPOPKeys[clientfd];
    auto key_iter = std::find(client_keys_list.begin(), client_keys_list.end(), list_key);
    client_keys_list.erase(key_iter);
}

void Lists::delete_list_keys_by_client_fd(int clientfd) {
    if (this->mapClientsBLPOPKeys.count(clientfd) == 0) return;
    std::vector<std::string> client_keys_lists = this->mapClientsBLPOPKeys[clientfd];
    for (std::string key: client_keys_lists) {
        std::vector<int> &clientfd_list = this->mapKeysClientsBLPOP[key];
        auto fd_it = std::find(clientfd_list.begin(), clientfd_list.end(), clientfd);
        clientfd_list.erase(fd_it);
    }
    this->mapClientsBLPOPKeys.erase(clientfd);
};

bool Lists::check_and_delete_client_waiting(const std::string& list_key, int clientfd) {
    std::vector<int>& clientfd_list = this->mapKeysClientsBLPOP[list_key];
    auto fd_it = std::find(clientfd_list.begin(), clientfd_list.end(), clientfd);
    if (fd_it != clientfd_list.end()) {
        clientfd_list.erase(fd_it);
        delete_list_key_by_client_fd(list_key, clientfd);
        return true;
    }
    return false;
}

int Lists::get_first_client(const std::string& list_key) {
    std::vector<int> &clientfd_list = this->mapKeysClientsBLPOP[list_key];
    int clientfd = clientfd_list.front();
    delete_list_key_by_client_fd(list_key, clientfd);
    clientfd_list.erase(clientfd_list.begin());
    return clientfd;
};