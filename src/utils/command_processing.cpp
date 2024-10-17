#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <map>
#include <any>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/array_parser_dec.h"
#include "decode/small_aggregate_parser_dec.h"
#include "process_rdbfile_datas.h"
#include "decode/simple_data_parser_dec.h"
#include "command_processing.h"
#include "global_datas.h"




std::vector<std::any> vect_getack = {std::string("REPLCONF"), std::string("GETACK"), std::string("*")};
std::string REPLCONF_GETACK_CMD = parse_encode_array(vect_getack);

void CommandProcessing::erase_key(const std::string& key) {
    GlobalDatas::dict_table.erase(key);
}

void CommandProcessing::execute_after_delay(int delay, const std::string& key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    erase_key(key);
}

void CommandProcessing::type(std::string key, int dest_fd){
    if (GlobalDatas::dict_table.count(key) != 0)
        send_data(std::string("+string\r\n"), dest_fd);
    else if (GlobalDatas::get_entry_index(key) != GlobalDatas::entries.size()){
        send_data(std::string("+stream\r\n"), dest_fd);
    } else  {
        send_data(std::string("+none\r\n"), dest_fd);
    }
}

bool CommandProcessing::send_data(std::string data, int dest_fd){
    if (!data.empty()){
        if (int s = send(dest_fd, data.c_str(), data.length(), 0) <= 0){
            std::cout <<  "send data failed";
            std::strerror(errno);
            return false;
        }
        return true;      
    }
    return false;
}

void CommandProcessing::echo(std::vector<std::string> extras, int dest_fd){
    if (extras.size() == 1){
        std::string resp = parse_encode_bulk_string(extras[0]);
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::ping(int dest_fd){
    std::string resp = parse_encode_bulk_string(std::string("PONG"));
    send_data(resp, dest_fd);
}

bool CommandProcessing::set_without_send(std::vector<std::string> extras){
    if (extras.size() > 1){
        std::string key = extras[0];
        GlobalDatas::set(key, extras[1]);
        if (extras.size() == 4){
            std::string param = extras[2];
            if (param == "px"){
                const int duration = stoi(extras[3]);
                std::thread t(execute_after_delay, duration, key);
                t.detach();
                return true;
            } return true;
        }
        return true;
    }
    return false;
}

void CommandProcessing::set(std::vector<std::string> extras, std::string data, int dest_fd){
    if (GlobalDatas::isMaster){
        for (auto& replica_fd_offset: GlobalMasterDatas::replicas_offsets) {
            if (send(replica_fd_offset.first, data.c_str(), data.length(), 0) <= 0)
                std::cout <<  "replica send msg failed";                
        }
        GlobalMasterDatas::set_commands_offset(data.size(), true);
    }
    if (set_without_send(extras)){
        std::string resp = parse_encode_simple_string("OK");
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::keys(std::vector<std::string> extras, int dest_fd, std::string filepath) {
    if (extras.size() == 1 && extras[0] == "*"){
        auto keys_values = get_keys_values_from_file(filepath);
        auto keys = keys_values.first;
        std::string resp = parse_encode_array(keys);
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::get(std::vector<std::string> extras, int dest_fd, std::string filepath){
    if (extras.size() == 1){
        std::string key = extras[0];
        auto keys_values = get_keys_values_from_file(filepath);
        int index = 0, size = keys_values.first.size();
        while (index < size && std::any_cast<std::string>(keys_values.first[index]) != key){
            index++;
        }
        std::string resp;
        if (index >= size || size == 0){
            if (GlobalDatas::dict_table.count(key) == 0)
                resp = "$-1\r\n";
            else 
                resp = parse_encode_bulk_string(GlobalDatas::get(key));
        }
        else
            resp = parse_encode_bulk_string(std::any_cast<std::string>(keys_values.second[index]));
        send_data(resp, dest_fd);
    }
    
}

std::pair<unsigned long, unsigned int> CommandProcessing::split_entry_id(std::string str){
    int ind_separator = str.find("-");

    try {
        unsigned long millisecond_time = std::stol(str.substr(0, ind_separator));
        unsigned int sequence_num = std::stoi(str.substr(ind_separator+1));

        return std::make_pair(millisecond_time, sequence_num);
    }
    catch(std::exception e){
        std::cout << e.what() " \n";
        return std::make_pair(1, 2);    
    }

};

void CommandProcessing::xadd(std::vector<std::string> extras, int dest_fd){
    if (extras.size() % 2 == 0){
        int size = GlobalDatas::entries.size();
        std::string str_error;
        auto new_entry_id = split_entry_id(extras[1]);
        if (new_entry_id.first == 0 && new_entry_id.second == 0){
            str_error = "ERR The ID specified in XADD must be greater than 0-0";
            send_data(parse_encode_error_msg(str_error), dest_fd);
            return;
        }   
        if (size > 0){
            auto last_entry = GlobalDatas::entries.back();
            auto last_entry_id = split_entry_id(last_entry.first);

            if (new_entry_id.first < last_entry_id.first || ((last_entry_id.first == new_entry_id.first) &&
            (new_entry_id.second <= last_entry_id.second)))  {
                str_error = "ERR The ID specified in XADD is equal or smaller than the target stream top item";
                send_data(parse_encode_error_msg(str_error), dest_fd);
                return;
            }     
        }
        GlobalDatas::set_entry(extras);
        std::string resp = parse_encode_bulk_string(extras[1]);
        send_data(resp, dest_fd);
    }
};


void CommandProcessing::config(std::vector<std::string> extras, int dest_fd, std::map<std::string, std::string> args_map){
    std::string resp;
    if (extras[0] == "get"){
        std::string key = extras[1];
        if (key == "dir"){
            std::vector<std::any> values = {key, args_map["dir"]};
            resp = parse_encode_array(values);
        }
        else if (key == "dbfilename"){
            std::vector<std::any> values = {key, args_map["dbfilename"]};
            resp = parse_encode_array(values);          
        }
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::info(std::vector<std::string> extras, int dest_fd, std::string role){
    if (extras[0] == "replication"){
        std::string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
        int replication_offset = 0;
        std::string str = "role:"+role + "\n"+ "master_replid:"+replication_id + 
            "\n" + "master_repl_offset:"+ std::to_string(replication_offset);
        std::string resp = parse_encode_bulk_string(str);
        send_data(resp, dest_fd);
    }  
}

void CommandProcessing::wait(unsigned int numreplicas, unsigned long timeout, int dest_fd){
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
    send_data(resp, dest_fd);
}

int64_t CommandProcessing::get_now_time_milliseconds() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void CommandProcessing::replconf(std::vector<std::string> extras, int dest_fd){
    if (extras[0] == "listening-port" || extras[0] == "capa" && extras.size() > 1){
        std::string resp = parse_encode_simple_string(std::string("OK"));
        send_data(resp, dest_fd);
    } else if (extras[0] == "getack" && extras[1] == "*"){
        std::vector<std::any> rep = {std::string("REPLCONF"), std::string("ACK"), std::to_string(GlobalDatas::prec_commands_offset)};
        std::string resp = parse_encode_array(rep);
        send_data(resp, dest_fd);
    }
    else if (extras[0] == "ack" && extras.size() == 2){
        try {
            int num = stoi(extras[1]);
            GlobalMasterDatas::replicas_offsets[dest_fd] = num;
        }
        catch(std::exception e){}
    }
}

void CommandProcessing::psync(std::vector<std::string> extras, int dest_fd){
    if (extras[0] == "?" && extras[1] == "-1"){
        std::string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
        std::string resp = parse_encode_simple_string("FULLRESYNC " + replication_id + " 0");
        std::string file_content_str = "524544495330303131fa0972656469732d76657205372e322e30fa0a72656469732d62697473c040fa056374696d65c26d08bc65fa08757365642d6d656dc2b0c41000fa08616f662d62617365c000fff06e3bfec0ff5aa2";
        std::vector<unsigned char> bytes = string_to_binary(file_content_str);
        const size_t size = bytes.size();
        unsigned char data[size];
        memcpy(data, bytes.data(), size);
        if (send_data(resp, dest_fd))
            if (send(dest_fd, data, size, 0) > 0){
                GlobalMasterDatas::replicas_offsets[dest_fd] = 0;
            }
    }   
}

std::pair<std::string, std::vector<std::string>> CommandProcessing::get_command_array_from_rawdata(std::string data){
    ArrayResp arr_resp = parse_decode_array(data);
    auto arr = std::get<ArrayAndInd>(arr_resp.first);
    auto command = arr.first;
    std::string cmd = std::any_cast<std::string>(command[0]);
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    std::vector<std::string> array_cmd;
    for (int i = 1; i < command.size(); i++){
        std::string param = std::any_cast<std::string>(command[i]);
        std::transform(param.begin(), param.end(), param.begin(), ::tolower);
        array_cmd.push_back(param);
    }
    return std::make_pair(cmd, array_cmd);
};

std::pair<std::string, std::vector<std::any>> CommandProcessing::get_command_array_multitypes_from_rawdata(std::string data){
    ArrayResp arr_resp = parse_decode_array(data);
    auto arr = std::get<ArrayAndInd>(arr_resp.first);
    auto command = arr.first;
    std::string cmd = std::any_cast<std::string>(command[0]);
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    std::vector<std::any> array_cmd;
    for (int i = 1; i < command.size(); i++){
        array_cmd.push_back(command[i]);
    }
    return std::make_pair(cmd, array_cmd);
};

void CommandProcessing::send_replconf_getack(int dest_fd){
    send_data(REPLCONF_GETACK_CMD, dest_fd);
};
