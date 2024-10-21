#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/simple_data_parser_dec.h"
#include "command_processing.h"
#include "stream_commands_processing.h"
#include "global_datas.h"


void StreamCommandsProcessing::xadd(std::vector<std::string> extras, int dest_fd){
    int index_entry = GlobalDatas::get_entry_index(extras[0]);
    std::string id;
    std::string str_error;

    if (extras[1] == "*"){
        long timestamp = CommandProcessing::get_now_time_milliseconds();
        id = std::to_string(timestamp) + "-0";
        extras[1] = id;
    }
    else {
        auto new_entry_id_str = split_entry_id(extras[1]);
        long new_ms_time = std::stol(new_entry_id_str.first);
        if (new_ms_time == 0 && new_entry_id_str.second != "*" && std::stol(new_entry_id_str.second) == 0){
            str_error = "ERR The ID specified in XADD must be greater than 0-0";
            CommandProcessing::send_data(parse_encode_error_msg(str_error), dest_fd);
            return;
        }
        int index_entry = GlobalDatas::get_entry_index(extras[0]);        
        if (index_entry < GlobalDatas::entries.size()){
            auto last_entry = GlobalDatas::entries[index_entry].second.back();
            auto last_entry_id = split_entry_id(last_entry["id"]);
            long last_entry_ms_time = std::stol(last_entry_id.first);
            int last_entry_seq_num = std::stol(last_entry_id.second);
            if (last_entry_ms_time > new_ms_time){
                str_error = "ERR The ID specified in XADD is equal or smaller than the target stream top item";
                CommandProcessing::send_data(parse_encode_error_msg(str_error), dest_fd);
                return;
            }
            else if (last_entry_ms_time == new_ms_time) {
                if (new_entry_id_str.second == "*"){
                    id = new_entry_id_str.first + "-" + std::to_string(last_entry_seq_num+1);
                    extras[1] = id;
                } else {
                    int new_seq_number = std::stoi(new_entry_id_str.second);
                    if (new_seq_number <= last_entry_seq_num){
                        str_error = "ERR The ID specified in XADD is equal or smaller than the target stream top item";
                        CommandProcessing::send_data(parse_encode_error_msg(str_error), dest_fd);
                        return;
                    }  
                }
            } else if (new_entry_id_str.second == "*"){
                 id = new_entry_id_str.first + "-0";
                 extras[1] = id;
            } 
        }
        else if (new_entry_id_str.second == "*"){
            id = new_entry_id_str.first + "-" + (new_ms_time == 0 ? "1" : "0");
            extras[1] = id;
        }  
    }
    GlobalDatas::set_entry(extras);
    std::string resp = parse_encode_bulk_string(extras[1]);
    CommandProcessing::send_data(resp, dest_fd);
        
};


void StreamCommandsProcessing::xrange(std::vector<std::string> extras, int dest_fd) {
    std::string entry_key = extras[0];
    std::pair<unsigned long, unsigned int> range_inf_id;
    std::pair<unsigned long, unsigned int> range_sup_id;
    if (extras[1] == "-"){
        range_inf_id = std::make_pair(0, 0);
        range_sup_id = split_entry_id_num(extras[2]);
    }
    else if (extras[2] == "+"){
        range_inf_id = split_entry_id_num(extras[1]);
        range_sup_id = std::make_pair(CommandProcessing::get_now_time_milliseconds() + 5000000, 0);
    }
    else {
        range_inf_id = split_entry_id_num(extras[1]);
        range_sup_id = split_entry_id_num(extras[2]);
    }
    int index_entry = GlobalDatas::get_entry_index(entry_key);
    VectorMapEntries entry_data_filtered;
    if (index_entry < GlobalDatas::entries.size()){
        auto entry_data = GlobalDatas::entries[index_entry].second;
        VectorMapEntries::iterator it = entry_data.begin();
        auto elt_id = split_entry_id_num((*it)["id"]);
        while ((elt_id.first < range_inf_id.first) || ((elt_id.first == range_inf_id.first) && (elt_id.second < range_inf_id.second)))
        {
            ++it;
            if (it != entry_data.end()){
                elt_id = split_entry_id_num((*it)["id"]);
            }     
            else break;        
        }
        if (it != entry_data.end())
            while ((elt_id.first < range_sup_id.first) || ((elt_id.first == range_sup_id.first) && (elt_id.second <= range_sup_id.second)))
            {    
                entry_data_filtered.push_back(*it);
                ++it;
                if (it != entry_data.end())
                    elt_id = split_entry_id_num((*it)["id"]);
                else break;
            }
    }
    std::string resp = parse_encode_array_for_xrange(entry_data_filtered);
    CommandProcessing::send_data(resp, dest_fd);
};

void StreamCommandsProcessing::xread(std::vector<std::string> extras, int dest_fd) {

    unsigned short nb_keys = (extras.size() - 1) / 2;
    std::vector<std::pair<std::string, VectorMapEntries>> keys_and_vectors_map;

    for (short i = 1; i <= nb_keys; i++ ){
        std::string key = extras[i];
        std::string value = extras[i+nb_keys];
        auto key_value_pair = split_entry_id_num(value);
        int index_entry = GlobalDatas::get_entry_index(key);
        VectorMapEntries entry_data_filtered;
        if (index_entry < GlobalDatas::entries.size()){
            auto entry_data = GlobalDatas::entries[index_entry].second;
            VectorMapEntries::iterator it = entry_data.begin();
            auto elt_id = split_entry_id_num((*it)["id"]);
            while ((elt_id.first < key_value_pair.first) || ((elt_id.first == key_value_pair.first) && (elt_id.second <= key_value_pair.second)))
            {
                ++it;
                if (it != entry_data.end()){
                    elt_id = split_entry_id_num((*it)["id"]);
                }     
                else break;        
            }
            while (it != entry_data.end()){
                entry_data_filtered.push_back(*it);
                ++it;               
            }
            if (entry_data_filtered.size() != 0)
                keys_and_vectors_map.push_back(std::make_pair(key, entry_data_filtered));
        }
    }
    std::string resp = parse_encode_array_for_xread(keys_and_vectors_map);
    CommandProcessing::send_data(resp, dest_fd);
};


void StreamCommandsProcessing::xread_with_block(std::vector<std::string> extras, int dest_fd) {
    int delay = std::stoi(extras[1]);
    auto first_elt = extras.begin();
    extras.erase(first_elt, first_elt+2);
    unsigned short nb_keys = (extras.size() - 1) / 2;
    for (int i = 1; i <= nb_keys; i++){
        if (extras[i+nb_keys] == "$"){
            int index_entry = GlobalDatas::get_entry_index(extras[i]);
            if (index_entry < GlobalDatas::entries.size()){
                auto last_elt = GlobalDatas::entries[index_entry].second.back();
                extras[i+nb_keys] = last_elt["id"];
            }
            else 
                extras[i+nb_keys] = "0-0";
        }
    }
    if (delay == 0){
        auto init_entry = GlobalDatas::get_entries();
        while (init_entry.size() == GlobalDatas::get_entries().size()){}
        std::cout << "aerty,ujyjhgjjjjjjjjjjj";
    }
    else {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    xread(extras, dest_fd);

};

std::pair<std::string, std::string> StreamCommandsProcessing::split_entry_id(std::string str){
    size_t ind_separator = str.find("-");
    return std::make_pair(str.substr(0, ind_separator), str.substr(ind_separator+1));
};

std::pair<unsigned long, unsigned int> StreamCommandsProcessing::split_entry_id_num(std::string str){
    size_t ind_separator = str.find("-");
    if (ind_separator == std::string::npos)
        return std::make_pair(std::stol(str), 0);
    return std::make_pair(std::stol(str.substr(0, ind_separator)), std::stoi(str.substr(ind_separator+1)));
};