#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/array_parser_dec.h"
#include "decode/small_aggregate_parser_dec.h"
#include "process_rdbfile_datas.h"
#include "decode/simple_data_parser_dec.h"
#include "command_processing.h"

using namespace std;


void CommandProcessing::erase_key(const std::string& key) {
    this->dict_table.erase(key);
}

void CommandProcessing::execute_after_delay(int delay, const std::string& key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    this->erase_key(key);
}

bool CommandProcessing::send_data(string data, const int& dest_fd){
    if (!data.empty()){
        if (send(dest_fd, data.c_str(), data.length(), 0) <= 0){
            cout <<  "send data failed";
            return false;
        }
        return true;      
    }
    return false;
}

void CommandProcessing::echo(vector<string> extras, const int& dest_fd){
    if (extras.size() == 1){
        string resp = parse_encode_bulk_string(extras[0]);
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::ping(const int& dest_fd){
    string resp = parse_encode_bulk_string(std::string("PONG"));
    send_data(resp, dest_fd);
}

bool CommandProcessing::set_without_send(vector<string> extras){
    if (extras.size() > 1){
        string key = extras[0];
        this->dict_table[key] = extras[1];
        if (extras.size() == 4){
            string param = extras[2];
            transform(param.begin(), param.end(), param.begin(), ::tolower);
            if (param == "px"){
                const int duration = stoi(extras[3]);
                thread t([this, &duration, &key]() {execute_after_delay(duration, key);});
                t.detach();
                return true;
            }
        }
    }
    return false;
}

void CommandProcessing::set(vector<string> extras, const int& dest_fd){
    if (this->set_without_send(extras)){
        string resp = parse_encode_simple_string("OK");
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::keys(vector<string> extras, const int& dest_fd, string filepath) {
    if (extras.size() == 1 && extras[0] == "*"){
        auto keys_values = get_keys_values_from_file(filepath);
        auto keys = keys_values.first;
        string resp = parse_encode_array(keys);
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::get(vector<string> extras, const int& dest_fd, string filepath){
    if (extras.size() == 1){
        std::string key = extras[0];
        auto keys_values = get_keys_values_from_file(filepath);
        int index = 0, size = keys_values.first.size();
        while (index < size && std::any_cast<std::string>(keys_values.first[index]) != key){
            index++;
        }
        string resp;
        if (index >= size || size == 0){
            if (this->dict_table.count(key) == 0)
                resp = "$-1\r\n";
            else 
                resp = parse_encode_bulk_string(this->dict_table[key]);
        }
        else
            resp = parse_encode_bulk_string(std::any_cast<std::string>(keys_values.second[index]));
        send_data(resp, dest_fd);
    }
    
}

void CommandProcessing::config(vector<string> extras, const int& dest_fd, std::map<std::string, std::string> args_map){
    string resp;
    if (extras[0] == "get"){
        string key = extras[1];
        if (key == "dir"){
            vector<any> values = {key, args_map["dir"]};
            resp = parse_encode_array(values);
        }
        else if (key == "dbfilename"){
            vector<any> values = {key, args_map["dbfilename"]};
            resp = parse_encode_array(values);          
        }
        send_data(resp, dest_fd);
    }
    
}

void CommandProcessing::info(vector<string> extras, const int& dest_fd, string role){
    if (extras[0] == "replication"){
        string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
        int replication_offset = 0;
        string str = "role:"+role + "\n"+ "master_replid:"+replication_id + 
            "\n" + "master_repl_offset:"+ to_string(replication_offset);
        string resp = parse_encode_bulk_string(str);
        send_data(resp, dest_fd);
    }  
}

void CommandProcessing::replconf(vector<string> extras, const int& dest_fd){
    if (extras[0] == "listening-port" || extras[0] == "capa" && extras.size() > 1){
        string resp = parse_encode_simple_string(string("OK"));
        send_data(resp, dest_fd);
    } else if (extras[0] == "getack" && extras[1] == "*"){
        string resp = parse_encode_simple_string(string("REPLCONF ACK 0"));
        send_data(resp, dest_fd);
    }
}

void CommandProcessing::psync(vector<string> extras, const int& dest_fd, vector<int>& replicas_fd){
    if (extras[0] == "?" && extras[1] == "-1"){
        string replication_id = "8371b4fb1155b71f4a04d3e1bc3e18c4a990aeeb";
        string resp = parse_encode_simple_string("FULLRESYNC " + replication_id + " 0");
        string file_content_str = "524544495330303131fa0972656469732d76657205372e322e30fa0a72656469732d62697473c040fa056374696d65c26d08bc65fa08757365642d6d656dc2b0c41000fa08616f662d62617365c000fff06e3bfec0ff5aa2";
        vector<unsigned char> bytes = string_to_binary(file_content_str);
        const size_t size = bytes.size();
        unsigned char data[size];
        memcpy(data, bytes.data(), size);
        if (send_data(resp, dest_fd))
            if (send(dest_fd, data, size, 0) > 0);
                replicas_fd.push_back(dest_fd);
    }   
}


