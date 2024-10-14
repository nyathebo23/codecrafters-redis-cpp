#ifndef COMMAND_PROCESSING_H
#define COMMAND_PROCESSING_H

#include <string>
#include <map>
#include <vector>


class CommandProcessing {
    protected:
        static void erase_key(const std::string& key);
        static void execute_after_delay(int delay, const std::string& key);
        

    public:
        static std::map<std::string, std::string> dict_table;
        void echo(std::vector<std::string> extras, int dest_fd);
        bool send_data(std::string data, int dest_fd);
        void ping(int dest_fd);
        void set(std::vector<std::string> extras, int dest_fd);
        bool set_without_send(std::vector<std::string> extras);
        void get(std::vector<std::string> extras, int dest_fd, std::string filepath);
        void keys(std::vector<std::string> extras, int dest_fd, std::string filepath);
        void info(std::vector<std::string> extras, int dest_fd, std::string role);
        void replconf(std::vector<std::string> extras, int dest_fd);
        void psync(std::vector<std::string> extras, int dest_fd, std::vector<int>& replicas_fd);
        void config(std::vector<std::string> extras, int dest_fd, std::map<std::string, std::string> args_map);
};

#endif