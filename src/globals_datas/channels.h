#ifndef CHANNELS_H
#define CHANNELS_H

#include <string>
#include <map>
#include <vector>

class Channels {
    private:
        std::map<std::string, std::vector<int>> mapChannelClientsFD;
        std::map<int, std::vector<std::string>> mapClientFDChannels;
        template <typename T>
        bool static remove_from_vector(std::vector<T>& vec, const T& item);

    public:
        std::size_t subscribe(const std::string& channelName, const int& sockfd);
        std::size_t unsubscribe(const std::string& channelName, const int& sockfd);
        std::vector<int> get_clients_fd(const std::string& channelName);
};

#endif