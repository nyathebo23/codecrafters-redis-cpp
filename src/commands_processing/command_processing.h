#ifndef COMMAND_PROCESSING_H
#define COMMAND_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include "../utils/decode/decoders.h"

class Command {
    public:
        std::string name;
        std::vector<std::string> args;
        std::optional<std::string> error;
        Command(std::string name, std::vector<std::string> params): name(name), args(params), error(std::nullopt) {}
        Command(std::string error): error(error) {}    
};

class CommandProcessing {

    protected:
        static void erase_key(const std::string& key);
        static void execute_after_delay(const int& delay, const std::string& key);

    public:
        static int64_t get_now_time_milliseconds();
        static Command get_command_array_from_rawdata(const std::string& data);
        static std::optional<Command> receive_command_from_client(const int& clientfd);
        static std::string get_command_response(const std::string& cmd, const std::vector<std::string>& extra_params, 
           const std::string& data, const int& clientfd, const std::map<std::string, std::string>& extra_args);
        static std::string params_count_error(const std::string& cmdname);
        static std::string echo(const std::vector<std::string>& extras);
        static bool send_data(const std::string& data, const int& dest_fd);
        static void send_replconf_getack(const int& dest_fd);
        static std::string ping();
        static std::string set(const std::vector<std::string>& extras, const std::string& data);
        static bool set_without_send(const std::vector<std::string>& extras);
        static std::string get(const std::vector<std::string>& extras, const std::string& filepath);
        static std::string keys(const std::vector<std::string>& extras, const std::string& filepath);
        static std::string info(const std::vector<std::string>& extras, const std::string& role);
        static std::string replconf(const std::vector<std::string>& extras, const int& destfd);
        static std::string wait(const std::vector<std::string>& extras);
        static void multi(const int& clientfd, const std::map<std::string, std::string>& extra_args);
        static void psync(const std::vector<std::string>& extras, const int& destfd);
        static std::string type(const std::vector<std::string>& extras);
        static std::string incr(const std::vector<std::string>& extras);
        static std::string config(const std::vector<std::string>& extras, const std::map<std::string, std::string>& args_map);
        static void process_file_datas(const int& dest_fd);

};


#endif