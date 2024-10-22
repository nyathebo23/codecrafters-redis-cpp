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

    public:
        static int64_t get_now_time_milliseconds();
        static std::pair<std::string, std::vector<std::string>> get_command_array_from_rawdata(std::string data);
        static std::pair<std::string, std::vector<std::any>> get_command_array_multitypes_from_rawdata(std::string data);
        static std::string echo(std::vector<std::string> extras);
        static bool send_data(std::string data, int dest_fd);
        static std::string send_replconf_getack(int dest_fd);
        static std::string ping();
        static std::string set(std::vector<std::string> extras, std::string data);
        static bool set_without_send(std::vector<std::string> extras);
        static std::string get(std::vector<std::string> extras, std::string filepath);
        static std::string keys(std::vector<std::string> extras, std::string filepath);
        static std::string info(std::vector<std::string> extras, std::string role);
        static std::string replconf(std::vector<std::string> extras);
        static std::string wait(unsigned int numreplicas, unsigned long timeout);
        static std::string psync(std::vector<std::string> extras);
        static std::string type(std::string key);
        static std::string incr(std::string key);
        static std::string config(std::vector<std::string> extras, std::map<std::string, std::string> args_map);
        static void process_file_datas(int dest_fd);

};

#endif