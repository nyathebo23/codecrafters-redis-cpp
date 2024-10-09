#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <thread>
#include <any>
#include <cmath>
#include <map>
#include <fstream>
#include <bitset>
#include <sstream>
#include "handle_connection.h"
#include "utils/encode/array_parser_enc.h"
#include "utils/encode/simple_data_parser_enc.h"
#include "utils/encode/small_aggregate_parser_enc.h"
#include "utils/decode/array_parser_dec.h"
#include "utils/decode/small_aggregate_parser_dec.h"
#include "utils/process_rdbfile_datas.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

std::map<std::string, std::string> dict_data;
std::string trim_low(const std::string& str);
// Fonction à exécuter
void erase_key(const std::string& key) {
    dict_data.erase(key);
}

// Fonction qui démarre un thread pour exécuter maFonction après un délai
void execute_after_delay(int delay, const std::string& key) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    erase_key(key);
}

void handle_connection(const int& clientfd, std::map<std::string, std::string> args_map){
  while (1) {
    char buffer[128];    
    if (recv(clientfd, &buffer, sizeof(buffer), 0) <= 0) {
      close(clientfd);
      return;
    }
    std::string data(buffer);
    ArrayResp arresp = parse_decode_array(data);
    auto arr = std::get<ArrayAndInd>(arresp.first);
    auto vals = arr.first;
    if (vals[0].type() == typeid(std::string)){
        std::string cmd = std::any_cast<std::string>(vals[0]);
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        std::string res;
        if (cmd == "echo"){
            if (vals.size() == 2 && vals[1].type() == typeid(std::string)){
               res = parse_encode_bulk_string(std::any_cast<std::string>(vals[1]));
            }
        }
        else if (cmd == "ping"){
            if (vals.size() == 1){
               res = parse_encode_bulk_string("PONG");
            }
        }
        else if (cmd == "set"){
            if (vals.size() > 2){
              const std::string key = std::any_cast<std::string>(vals[1]);
              dict_data[key] = std::any_cast<std::string>(vals[2]);
              res = parse_encode_simple_string("OK");
              if (vals.size() == 5 && vals[3].type() == typeid(std::string)){
                std::string param = std::any_cast<std::string>(vals[3]);
                std::transform(param.begin(), param.end(), param.begin(), ::tolower);
                if (param == "px"){
                    const int duration = std::stoi(std::any_cast<std::string>(vals[4]));
                    std::thread t(execute_after_delay, duration, key);
                    t.detach();
                }
              }
            }
        }
        else if (cmd == "get"){
            if (vals.size() == 2 && vals[1].type() == typeid(std::string)){
                std::string key = std::any_cast<std::string>(vals[1]);
                auto keys_values = get_keys_values_from_file(args_map["--dir"] + "/" + args_map["--dbfilename"]);
                auto keys = keys_values.first;
                auto values = keys_values.second;
                int index = 0, size = keys.size();
                while (index < size && std::any_cast<std::string>(keys[index]) != key){
                    index++;
                }
                if (index > size || size == 0){
                    if (dict_data.count(key) == 0)
                        res = "$-1\r\n";
                    else {
                        res = parse_encode_bulk_string(std::any_cast<std::string>(dict_data[key]))
                    }
                }
                else
                    res = parse_encode_bulk_string(std::any_cast<std::string>(values[index]));
            }
        }
        else if (cmd == "config"){
          std::string cmd2 = std::any_cast<std::string>(vals[1]);
          std::transform(cmd2.begin(), cmd2.end(), cmd2.begin(), ::tolower);
          if (cmd2 == "get"){
             std::string key = std::any_cast<std::string>(vals[2]);
             if (key == "dir"){
                std::vector<std::any> values = {key, args_map["--dir"]};
                res = parse_encode_array(values);
             }
             else if (key == "dbfilename"){
                std::vector<std::any> values = {key, args_map["--dbfilename"]};
                res = parse_encode_array(values);          
             }
          }
        }
        else if (cmd == "keys"){
            std::string param = std::any_cast<std::string>(vals[1]);
            //std::transform(param.begin(), param.end(), param.begin(), ::tolower);
            if (param == "*"){
                auto keys_values = get_keys_values_from_file(args_map["--dir"] + "/" + args_map["--dbfilename"]);
                auto keys = keys_values.first;
                res = parse_encode_array(keys);
            }
        }
        if (!res.empty())
            send(clientfd, res.c_str(), res.length(), 0);
    }
  }
}
