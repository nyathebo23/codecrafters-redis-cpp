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
        bool static remove_from_vector(std::vector<T> vec, const T& item);

    public:
        std::size_t subscribe(std::string channelName, int sockfd);
        std::size_t unsubscribe(std::string channelName, int sockfd);
        std::vector<int> get_clients_fd(std::string channelName);
};

#endif