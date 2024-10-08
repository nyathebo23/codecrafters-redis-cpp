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
            if (dict_data.count(key) == 0)
              res = "$-1\r\n";
            else
              res = parse_encode_bulk_string(std::any_cast<std::string>(dict_data[key]));
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
                //auto keys = get_keys_values_from_file(args_map["--dir"] + "/" + args_map["--dbfilename"]);
                std::ifstream input_file(args_map["--dir"] + "/" + args_map["--dbfilename"], std::ios::binary);
                std::stringstream ss;
                std::vector<char> buffer((std::istreambuf_iterator<char>(input_file)),
                                        std::istreambuf_iterator<char>());
                std::vector<std::any> keys;
                int index = 0, buffer_size = buffer.size();
                while (index < buffer_size && (int)(unsigned char)buffer[index] != 254){
                    index++;
                }
                while (index < buffer_size && (int)(unsigned char)buffer[index] != 251){
                    index++;
                }

                while (index < buffer_size && (int)(unsigned char)buffer[index] != 255){
                    if ((int)(unsigned char)buffer[index] == 253) { 
                        index += 4;
                        get_key_name(buffer, index, keys);
                    }
                    if ((int)(unsigned char)buffer[index] == 252) { 
                        index += 8;
                        get_key_name(buffer, index, keys);
                    } 
                    if ((int)(unsigned char)buffer[index] == 0){

                    unsigned char ch = buffer[index];
                    int n = ch;
                    std::stringstream ss;
                    const std::string str_byte = std::bitset<8>(n).to_string();
                    int len;
                    index++;
                    if (str_byte.substr(0, 2) == "00"){
                        len = std::stoi(str_byte.substr(2, 6), nullptr, 2);
                        while (len > 0){
                            ss << buffer[index];
                            len --;
                            index ++;
                        }
                        keys.push_back(ss.str());    
                    }
                    else if (str_byte.substr(0, 2) == "01"){
                        ch = buffer[index]; n = ch;
                        len = std::stoi(str_byte.substr(2, 6) + std::bitset<8>(n).to_string(), nullptr, 2);
                        index++;
                        while (len > 0){
                            ss << buffer[index];
                            len --;
                            index ++;
                        }
                        keys.push_back(ss.str());    
                    }
                    else if (str_byte.substr(0, 2) == "10"){
                        std::string lenstr = "";
                        for (int i = 0; i < 4; i++){
                            ch = buffer[index]; n = ch; 
                            lenstr += std::bitset<8>(n).to_string();
                            index ++;
                        }
                        len = std::stoi(lenstr, nullptr, 2); 
                        while (len > 0){
                            index ++;
                            ss << buffer[index];
                            len --;
                        }   
                        keys.push_back(ss.str());    
                    }


                    }
                                        int u = (int)(unsigned char)buffer[index];
                    keys.push_back(std::to_string(u));
                }
            res = parse_encode_array(keys);

          }
            if (!res.empty())
                send(clientfd, res.c_str(), res.length(), 0);
        }
    }
  }
}
