#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "../utils/encode/array_parser_enc.h"
#include "../utils/encode/simple_data_parser_enc.h"
#include "../utils/encode/small_aggregate_parser_enc.h"
#include "../utils/encode/encoders.h"
#include "../utils/decode/array_parser_dec.h"
#include "../utils/decode/small_aggregate_parser_dec.h"
#include "../utils/process_rdbfile_datas.h"
#include "../utils/decode/simple_data_parser_dec.h"
#include "command_processing.h"
#include "stream_commands_processing.h"
#include "list_commands_processing.h"
#include "sortedsets_commands_processing.h"
#include "geospatial_commands_processing.h"
#include "../globals_datas/global_datas.h"
#include "../utils/resp_constants.h"



void CommandProcessing::erase_key(const std::string& key) {
    GlobalDatas::datas_dict.erase(key);
}

void CommandProcessing::execute_after_delay(const int& delay, const std::string& key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    erase_key(key);
}

std::string CommandProcessing::params_count_error(const std::string& cmdname) {
    return parse_encode_error_msg(cmdname + " command format error");
};

std::string CommandProcessing::type(const std::vector<std::string>& extras){
    std::string resp = params_count_error("type");
    if (extras.size() != 1)
        return resp;
    std::string key = extras[0];
    if (GlobalDatas::datas_dict.exist(key))
        resp = STRING_TYPE_RESP;
    else if (GlobalDatas::stream_list.exist(key)){
        resp = STREAM_TYPE_RESP;
    } 
    else if (GlobalDatas::lists.exist(key)){
        resp = LIST_TYPE_RESP;
    } 
    else {
        resp = NONE_TYPE_RESP;
    }
    return resp;
}

bool CommandProcessing::send_data(const std::string& data, const int &dest_fd){
    if (!data.empty()){
        if (send(dest_fd, data.c_str(), data.length(), 0) <= 0){
            std::cout <<  "send data failed";
            std::strerror(errno);
            return false;
        }
        return true;      
    }
    return false;
}

std::string CommandProcessing::incr(const std::vector<std::string>& extras){
    std::string resp = params_count_error("incr");
    if (extras.size() != 1)
        return resp;
    std::string key = extras[0];
    if (!GlobalDatas::datas_dict.exist(key)){
        GlobalDatas::datas_dict.set(key, "1");
        resp = parse_encode_integer(1);
    }
    else {
        try {
            int new_num = std::stoi(GlobalDatas::datas_dict.get(key)) + 1;
            std::string new_num_str = std::to_string(new_num);
            GlobalDatas::datas_dict.set(key, new_num_str);
            resp = parse_encode_integer(new_num);
        }
        catch (const std::invalid_argument& e){
            resp = parse_encode_error_msg("value is not an integer or out of range");
        }
        catch (const std::out_of_range& e){
            resp = parse_encode_error_msg("value is not an integer or out of range");
        }
    }
    return resp;
}

std::string CommandProcessing::echo(const std::vector<std::string>& extras){
    std::string resp = params_count_error("echo");
    if (extras.size() == 1){
        resp = parse_encode_bulk_string(extras[0]);
    }
    return resp;
}

std::string CommandProcessing::ping(){
    return PONG_RESP;
}

bool CommandProcessing::set_without_send(const std::vector<std::string>& extras){
    if (extras.size() > 1){
        std::string key = extras[0];
        GlobalDatas::datas_dict.set(key, extras[1]);
        if (extras.size() == 4){
            std::string param = extras[2];
            if (param == "px"){
                const int DURATION = stoi(extras[3]);
                std::thread t(execute_after_delay, DURATION, key);
                t.detach();
                return true;
            } 
            return true;
        }
        return true;
    }
    return false;
}

std::string CommandProcessing::set(const std::vector<std::string>& extras, const std::string& data){
    if (GlobalDatas::is_master){
        for (auto& replica_fd_offset: GlobalMasterDatas::replicas_offsets) {
            if (send(replica_fd_offset.first, data.c_str(), data.length(), 0) <= 0)
                std::cout <<  "replica send msg failed";                
        }
        GlobalMasterDatas::set_commands_offset(data.size(), true);
    }
    std::string resp;
    if (set_without_send(extras)){
        resp = OK_RESP;
    }
    return resp;
}

std::string CommandProcessing::keys(const std::vector<std::string>& extras, const std::string& filepath) {
    std::string resp = params_count_error("keys");
    if (extras.size() == 1 && extras[0] == "*"){
        auto keys_values = get_keys_values_from_file(filepath);
        auto keys = keys_values.first;
        resp = parse_encode_string_array(keys);
    }
    return resp;
}

std::string CommandProcessing::get(const std::vector<std::string>& extras, const std::string& filepath){
    std::string resp = params_count_error("get");
    if (extras.size() == 1){
        std::string key = extras[0];
        auto keys_values = get_keys_values_from_file(filepath);
        int index = 0, size = keys_values.first.size();
        while (index < size && keys_values.first[index] != key){
            index++; 
        }
        if (index >= size || size == 0){
            if (!GlobalDatas::datas_dict.exist(key))
                resp = NULL_BULK_STRING;
            else 
                resp = parse_encode_bulk_string(GlobalDatas::datas_dict.get(key));
        }
        else
            resp = parse_encode_bulk_string(keys_values.second[index]);
    }
    return resp;    
}


std::string CommandProcessing::config(const std::vector<std::string>& extras, const std::map<std::string, std::string>& args_map){
    std::string resp = params_count_error("config");
    if (extras[0] == "get"){
        std::string key = extras[1];
        if (key == "dir"){
            std::vector<std::string> values = {key, args_map.at("dir")};
            resp = parse_encode_string_array(values);
        }
        else if (key == "dbfilename"){
            std::vector<std::string> values = {key, args_map.at("dbfilename")};
            resp = parse_encode_string_array(values);
        }
    }
    return resp;
}


std::string CommandProcessing::info(const std::vector<std::string>& extras, const std::string& role){
    std::string resp = params_count_error("info");
    if (extras.size() == 1 && extras[0] == "replication"){
        std::string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
        int replication_offset = 0;
        std::string str = "role:"+role + "\n"+ "master_replid:"+replication_id + 
            "\n" + "master_repl_offset:"+ std::to_string(replication_offset);
        resp = parse_encode_bulk_string(str);
    }
    return resp;  
}

std::string CommandProcessing::wait(const std::vector<std::string>& extras){
    if (extras.size() != 2) {
        return params_count_error("wait");
    }
    unsigned long numreplicas = std::stoul(extras[0]); 
    unsigned long timeout = std::stoul(extras[1]);
    int nb_replicas_updated = 0;
    if (GlobalMasterDatas::prec_commands_offset == GlobalMasterDatas::commands_offset){
        for (auto& replica_pair: GlobalMasterDatas::replicas_offsets)
            CommandProcessing::send_replconf_getack(replica_pair.first);
        if (GlobalMasterDatas::prec_commands_offset != 0)
            GlobalMasterDatas::set_commands_offset(REPLCONF_GETACK_CMD.size(), false);
    }
    std::vector<int> list_replicas_fd;
    for (auto&  replica_pair: GlobalMasterDatas::replicas_offsets)
        list_replicas_fd.push_back(replica_pair.first);

    auto limit = get_now_time_milliseconds() + timeout;
    while (get_now_time_milliseconds() < limit && nb_replicas_updated < numreplicas){
        for (auto&  replica_pair: GlobalMasterDatas::replicas_offsets)
            if (replica_pair.second == GlobalMasterDatas::prec_commands_offset){
                auto it = std::find(list_replicas_fd.begin(), list_replicas_fd.end(), replica_pair.first);
                if (it != list_replicas_fd.end()){
                    list_replicas_fd.erase(it);
                    nb_replicas_updated++;
                }
            }
    }
    std::string resp = parse_encode_integer(nb_replicas_updated);
    return resp;
}

int64_t CommandProcessing::get_now_time_milliseconds() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

std::string CommandProcessing::replconf(const std::vector<std::string>& extras, const int& dest_fd){
    std::string resp;
    if (extras.size() != 2) return params_count_error("replconf");
    if (extras[0] == "listening-port" ){
        resp = OK_RESP;
    } 
    else if (extras[0] == "capa" && extras[1] == "psync2"){
        resp = OK_RESP;
    }
    else if (extras[0] == "getack" && extras[1] == "*"){
        std::string cmd_offset_enc = parse_encode_bulk_string(std::to_string(GlobalDatas::cmds_offset.get_prec_cmd_offset()));
        send_data(REPLCONF_ACK_PART_ENC + cmd_offset_enc, dest_fd);
    }
    else if (extras[0] == "ack"){
        int num = stoi(extras[1]);
        GlobalMasterDatas::replicas_offsets[dest_fd] = num;
    }
    return resp;

}

void CommandProcessing::psync(const std::vector<std::string>& extras, const int& destfd){
    std::string resp = params_count_error("psync");
    if (extras.size() == 2 && extras[0] == "?" && extras[1] == "-1"){
        std::string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
        resp = parse_encode_simple_string("FULLRESYNC " + replication_id + " 0");
        CommandProcessing::send_data(resp, destfd);
        CommandProcessing::process_file_datas(destfd);
        return;
    }   
    CommandProcessing::send_data(resp, destfd);
}

void CommandProcessing::multi(const int& clientfd, std::map<std::string, std::string> extra_args) {
    CommandProcessing::send_data(OK_RESP, clientfd);
    std::vector<std::string> cmds_to_exec;
    while (1) {
        char buffer[1024] = {0};  
        int bytes_received = recv(clientfd, &buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            close(clientfd);
            break;
        }
        buffer[bytes_received] = '\0';
        std::string data(buffer);

        auto command = CommandProcessing::get_command_array_from_rawdata(data);
        if (command.error.has_value()){
            CommandProcessing::send_data(parse_encode_error_msg(command.error.value()), clientfd);
            continue;
        }
        std::string cmd = command.name;
        if (cmd == "exec"){
            if (cmds_to_exec.size() == 0){
                CommandProcessing::send_data("*0\r\n", clientfd);
            }
            else {
                std::string resp_array_str = "*" + std::to_string(cmds_to_exec.size()) + "\r\n";
                for (auto cmd_data: cmds_to_exec){
                    auto command_and_params = CommandProcessing::get_command_array_from_rawdata(cmd_data);
                    resp_array_str += get_command_response(command_and_params.name, command_and_params.args, cmd_data, clientfd, extra_args);
                }
                CommandProcessing::send_data(resp_array_str, clientfd);
                cmds_to_exec.clear();            
            }  
            return;      
        }
        else if (cmd == "discard") {           
            CommandProcessing::send_data(OK_RESP, clientfd);
            return;
        }
        else {
            cmds_to_exec.push_back(data);
            CommandProcessing::send_data(QUEUE_RESP, clientfd);
        }
    }
}

void CommandProcessing::process_file_datas(const int& dest_fd){
    std::string file_content_str = "524544495330303131fa0972656469732d76657205372e322e30fa0a72656469732d62697473c040fa056374696d65c26d08bc65fa08757365642d6d656dc2b0c41000fa08616f662d62617365c000fff06e3bfec0ff5aa2";
    std::vector<unsigned char> bytes = string_to_binary(file_content_str);
    const size_t SIZE = bytes.size();
    unsigned char data[SIZE];
    memcpy(data, bytes.data(), SIZE);
    if (send(dest_fd, data, SIZE, 0) > 0){
        GlobalMasterDatas::replicas_offsets[dest_fd] = 0;
    }
}

Command CommandProcessing::get_command_array_from_rawdata(const std::string& data){
    ArrayDecodeResult arr_resp = parse_decode_array(data);
    if (arr_resp.get_error().has_value()){
        return Command(arr_resp.get_error().value());
    }
    if (arr_resp.get_char_end_index() != data.length()){
        return Command("Command encode format error");
    }
    std::vector<DecodedResultPtr> arr = arr_resp.as_array();
    std::string cmd = arr[0]->as_string();
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    std::vector<std::string> array_cmd;
    for (int i = 1; i < arr.size(); i++){
        std::string param_str = arr[i]->as_string();
        std::transform(param_str.begin(), param_str.end(), param_str.begin(), ::tolower);
        array_cmd.push_back(param_str);
    }
    return Command(cmd, array_cmd);
};

std::optional<Command> CommandProcessing::receive_command_from_client(const int& clientfd) {
    char buffer[1024] = {0};  
    int bytes_received = recv(clientfd, &buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(clientfd);
        return std::nullopt;
    }

    buffer[bytes_received] = '\0';

    std::string data(buffer);
    return CommandProcessing::get_command_array_from_rawdata(data);
};


std::string CommandProcessing::get_command_response(const std::string&  cmd, const std::vector<std::string>& extra_params, 
    const std::string&  data, const int& clientfd, std::map<std::string, std::string> extra_args){
    if (cmd == "echo"){
        return CommandProcessing::echo(extra_params);
    }
    else if (cmd == "wait"){
        return CommandProcessing::wait(extra_params);
    }
    else if (cmd == "ping"){
        return CommandProcessing::ping();
    }
    else if (cmd == "incr"){
        return CommandProcessing::incr(extra_params);
    }
    else if (cmd == "type"){
        return CommandProcessing::type(extra_params);
    }
    else if (cmd == "xadd"){
        return StreamCommandsProcessing::xadd(extra_params);
    }
    else if (cmd == "xrange"){
        return StreamCommandsProcessing::xrange(extra_params);
    }
    else if (cmd == "xread"){
        if (extra_params.size() >= 1 && extra_params[0] == "block")
            return StreamCommandsProcessing::xread_with_block(extra_params);
        else
            return StreamCommandsProcessing::xread(extra_params);
    }
    else if (cmd == "lrange") {
        return ListCommandsProcessing::lrange(extra_params);
    }
    else if (cmd == "lpop") {
        return ListCommandsProcessing::lpop(extra_params);
    }
    else if (cmd == "llen") {
        return ListCommandsProcessing::llen(extra_params);
    }
    else if (cmd == "zadd") {
        return SortedSetsCommandsProcessing::zadd(extra_params);
    }
    else if (cmd == "zrange") {
        return SortedSetsCommandsProcessing::zrange(extra_params);
    }
    else if (cmd == "zrank") {
        return SortedSetsCommandsProcessing::zrank(extra_params);
    }
    else if (cmd == "zcard") {
        return SortedSetsCommandsProcessing::zcard(extra_params);
    }
    else if (cmd == "zscore") {
        return SortedSetsCommandsProcessing::zscore(extra_params);
    }
    else if (cmd == "zrem") {
        return SortedSetsCommandsProcessing::zrem(extra_params);
    }
    else if (cmd == "geoadd") {
        return GeospatialCommandsProcessing::geoadd(extra_params);
    }
    else if (cmd == "geopos") {
        return GeospatialCommandsProcessing::geopos(extra_params);
    }
    else if (cmd == "geodist") {
        return GeospatialCommandsProcessing::geodist(extra_params);
    }
    else if (cmd == "geosearch") {
        return GeospatialCommandsProcessing::geosearch(extra_params);
    }
    else if (cmd == "set") {
        return CommandProcessing::set(extra_params, data);
    }
    else if (cmd == "get"){
        return CommandProcessing::get(extra_params, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "config"){
        return CommandProcessing::config(extra_params, extra_args);
    }
    else if (cmd == "keys"){
        return CommandProcessing::keys(extra_params, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "info"){
        return CommandProcessing::info(extra_params, GlobalDatas::is_master ? "master" : "slave");
    }
    else if (cmd == "replconf"){
        return CommandProcessing::replconf(extra_params, clientfd);
    }

    return "-ERR\r\n";
};


void CommandProcessing::send_replconf_getack(const int& dest_fd){
    send_data(REPLCONF_GETACK_CMD, dest_fd);
};
