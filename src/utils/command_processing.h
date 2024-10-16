#ifndef COMMAND_PROCESSING_H
#define COMMAND_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include <any>


class CommandProcessing {
    protected:
        static void erase_key(const std::string& key);
        static void execute_after_delay(int delay, const std::string& key);
        static int64_t get_now_time_milliseconds();
    public:

        static std::pair<std::string, std::vector<std::string>> get_command_array_from_rawdata(std::string data);
        static std::pair<std::string, std::vector<std::any>> get_command_array_multitypes_from_rawdata(std::string data);
        static void echo(std::vector<std::string> extras, int dest_fd);
        static bool send_data(std::string data, int dest_fd);
        static void send_replconf_getack(int dest_fd);
        static void ping(int dest_fd);
        static void set(std::vector<std::string> extras, int dest_fd);
        static bool set_without_send(std::vector<std::string> extras);
        static void get(std::vector<std::string> extras, int dest_fd, std::string filepath);
        static void keys(std::vector<std::string> extras, int dest_fd, std::string filepath);
        static void info(std::vector<std::string> extras, int dest_fd, std::string role);
        static void replconf(std::vector<std::string> extras, int dest_fd);
        static void wait(unsigned int numreplicas, unsigned long timeout, int dest_fd);
        static void psync(std::vector<std::string> extras, int dest_fd);
        static void config(std::vector<std::string> extras, int dest_fd, std::map<std::string, std::string> args_map);
};

#endif