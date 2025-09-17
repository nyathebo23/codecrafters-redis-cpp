#include "channels.h"
#include <algorithm>

std::size_t Channels::subscribe(const std::string& channelName, const int& sockfd) {
    this->mapChannelClientsFD[channelName].push_back(sockfd);
    this->mapClientFDChannels[sockfd].push_back(channelName);
    return this->mapClientFDChannels[sockfd].size();
};

std::size_t Channels::unsubscribe(const std::string& channelName, const int& sockfd) {
    if (this->mapChannelClientsFD.count(channelName) == 0) {
        return 0;
    }
    if (Channels::remove_from_vector(this->mapChannelClientsFD[channelName], sockfd)){
        Channels::remove_from_vector(this->mapClientFDChannels[sockfd], channelName);
    };
    
    return this->mapClientFDChannels[sockfd].size();
};

template <typename T>
bool Channels::remove_from_vector(std::vector<T>& vec, const T& item) {
    auto it = std::find(vec.begin(), vec.end(), item);
    if (it != vec.end()) {
        vec.erase(it);
        return true;
    }
    return false;
};

std::vector<int> Channels::get_clients_fd(const std::string& channelName) {
    return this->mapChannelClientsFD[channelName];
};