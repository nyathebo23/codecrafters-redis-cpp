#ifndef COMMAND_PROCESSING_H
#define COMMAND_PROCESSING_H

#include <string>
#include <thread>
#include <map>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>


class CommandProcessing {
    protected:
        void erase_key(const std::string& key);
        void execute_after_delay(int delay, const std::string& key);
        std::map<std::string, std::string> dict_table;

    public:
        void echo(std::vector<std::string> extras, const int& dest_fd);
        bool send_data(std::string data, const int& dest_fd);
        void ping(const int& dest_fd);
        void set(std::vector<std::string> extras, const int& dest_fd);
        bool set_without_send(std::vector<std::string> extras);
        void get(std::vector<std::string> extras, const int& dest_fd, std::string filepath);
        void keys(std::vector<std::string> extras, const int& dest_fd, std::string filepath);
        void info(std::vector<std::string> extras, const int& dest_fd, std::string role);
        void replconf(std::vector<std::string> extras, const int& dest_fd);
        void psync(std::vector<std::string> extras, const int& dest_fd, std::vector<int>& replicas_fd);
        void config(std::vector<std::string> extras, const int& dest_fd, std::map<std::string, std::string> args_map);
};




#endif