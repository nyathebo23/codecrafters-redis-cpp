#include <iostream>
#include <string>
#include <cmath>
#include <variant>
#include <fstream>
#include <vector>
#include <any>
#include <sstream>
#include <bitset>
#include <filesystem>
#include "process_rdbfile_datas.h"
#include <cstring> 
#include <algorithm>
#include <chrono>

using str_nullable = std::variant<std::string, std::nullptr_t>;


bool is_file_empty(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::ate);  // Open the file in "ate" mode to move the cursor to the end
    return file.tellg() == 0;  // tellg() returns the position of the cursor, which will be 0 if the file is empty
}

std::string decode_str_length(int& index, std::vector<char>& buffer){
    unsigned char ch = buffer[index];
    int n = ch;
    std::stringstream ss;
    const std::string str_byte = std::bitset<8>(n).to_string();
    int len;
    index++;
    if (str_byte.substr(0, 2) == "00"){
        len = std::stoi(str_byte.substr(2, 6), nullptr, 2);
        while (len > 0){
            ss << (unsigned char) buffer[index];
            len --;
            index ++;
        }
        return  ss.str();
    }
    else if (str_byte.substr(0, 2) == "01"){
        ch = buffer[index]; 
        n = ch;
        len = std::stoi(str_byte.substr(2, 6) + std::bitset<8>(n).to_string(), nullptr, 2);
        index++;
        while (len > 0){
            ss << ch;
            len --;
            index ++;
        }
        return ss.str();
    }
    else if (str_byte.substr(0, 2) == "10"){
        std::string lenstr = "";
        for (int i = 0; i < 4; i++){
            ch = buffer[index]; 
            n = ch; 
            lenstr += std::bitset<8>(n).to_string();
            index ++;
        }
        len = std::stoi(lenstr, nullptr, 2); 
        while (len > 0){
            ss << (unsigned char) buffer[index];
            len --;
            index ++;
        }   
        return  ss.str();   
    }
    else {
        if (str_byte == "11000000"){
            ch = buffer[index]; n = ch;
            index++;
            return std::to_string(n);            
        } 
        else if (str_byte == "11000001"){
            char binary_num[] = {buffer[index+1], buffer[index]};
            int num;
            std::memcpy(&num, binary_num, sizeof(int)/2); 
            index += 2;
            return std::to_string(num);           
        }
        else if (str_byte == "11000010"){
            char binary_num[] = {buffer[index+3], buffer[index+2], buffer[index+1], buffer[index]};
            int num;
            std::memcpy(&num, binary_num, sizeof(int)); 
            index += 4;
            return std::to_string(num);           
        }
        else {
            return "azerty";
        }
    }
}

void get_key_value_pair(std::vector<char>& buffer, int &index, std::vector<std::any>& keys, std::vector<std::any>& values){
    if ((int)(unsigned char)buffer[index] == 0){
        index++;
        keys.push_back(decode_str_length(index, buffer));
        values.push_back(decode_str_length(index, buffer));
    }
    else
        index++;
}

enum byte_space {one_byte=1, two_bytes=2, four_bytes=4, eight_bytes=8};

bool check_key_date_validity(std::vector<char>& buffer, int &index, byte_space nb_bytes){
    unsigned char binary_num[nb_bytes];
    for (int j = 0; j < nb_bytes; j++)
        binary_num[j] = buffer[index+j+1];
    auto now = std::chrono::steady_clock::now();
    // Convert the current time to time since epoch
    auto duration = now.time_since_epoch();
    // Convert duration to milliseconds
    int64_t timestamp;
    if (nb_bytes == four_bytes){
        std::memcpy(&timestamp, binary_num, sizeof(int32_t));
        return timestamp > std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    }
    else {
        std::memcpy(&timestamp, binary_num, sizeof(int64_t));
        return timestamp > std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(); 
    }
}

std::pair<std::vector<std::any>, std::vector<std::any>> get_keys_values_from_file(std::string filepath){
    std::ifstream input_file(filepath, std::ios::binary);
    if (std::filesystem::is_directory(filepath) || !input_file.is_open())
        return {};
    std::vector<char> buffer((std::istreambuf_iterator<char>(input_file)),
                              std::istreambuf_iterator<char>());
    std::stringstream ss;
    std::vector<std::any> keys;
    std::vector<std::any> values;

    int index = 0, buffer_size = buffer.size();
    while (index < buffer_size && (int)(unsigned char)buffer[index] != 254){
        index++;
    }
    while (index < buffer_size && (int)(unsigned char)buffer[index] != 251){
        index++;
    }
    index += 3;
    while (index < buffer_size && (int)(unsigned char)buffer[index] != 255){
       if ((int)(unsigned char)buffer[index] == 253) { 
            if (check_key_date_validity(buffer, index, four_bytes)){
                // unsigned char binary_num[4];
                // for (int i = 4, j = 0; i > 0; i--, j++)
                //     binary_num[j] = buffer[index+i];
                // auto now = std::chrono::steady_clock::now();
                // // Convert the current time to time since epoch
                // auto duration = now.time_since_epoch();
                // // Convert duration to milliseconds
                // int64_t timestamp;
                // std::memcpy(&timestamp, binary_num, sizeof(int32_t));
                // keys.push_back(std::to_string(timestamp));
                // values.push_back(std::to_string(std::chrono::duration_cast<std::chrono::seconds>(duration).count()));
                index += 5;
                get_key_value_pair(buffer, index, keys, values);
            }
            continue;     
       }
       if ((int)(unsigned char)buffer[index] == 252) { 
            if (check_key_date_validity(buffer, index, eight_bytes)){
                unsigned char binary_num[8];
                unsigned char binary_num2[8];

                for (int j = 0; j < 8; j++)
                    binary_num[j] = buffer[index+j+1];
                for (int j = 0; j < 8; j++)
                    binary_num2[j] = binary_num[8-j-1];
                auto now = std::chrono::steady_clock::now();
                // Convert the current time to time since epoch
                auto duration = now.time_since_epoch();
                // Convert duration to milliseconds
                unsigned long timestamp;
                std::memcpy(&timestamp, binary_num2, sizeof(long));
                keys.push_back(std::to_string(timestamp));
                keys.push_back(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()));
                index += 9;
                get_key_value_pair(buffer, index, keys, values);
            }
            continue;           
       } 
        get_key_value_pair(buffer, index, keys, values);
    }
    input_file.close();
    return std::make_pair(keys, values);
}



// int main(int argc, char **argv) {
//     std::vector<std::any> str = get_keys_values_from_file("rdbfile.rdb");
//     std::cout << str.size();
//     for (int i = 0; i < str.size(); i++)
//         std::cout << std::any_cast<std::string>(str[i]);
//     return 0;
// }