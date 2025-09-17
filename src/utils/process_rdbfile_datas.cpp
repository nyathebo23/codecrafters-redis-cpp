#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <vector>
#include <sstream>
#include <bitset>
#include <filesystem>
#include "process_rdbfile_datas.h"
#include <cstring> 
#include <algorithm>
#include <chrono>


bool is_file_empty(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::ate);  // Open the file in "ate" mode to move the cursor to the end
    return file.tellg() == 0;  // tellg() returns the position of the cursor, which will be 0 if the file is empty
}

std::string decode_str_length(int& index, std::vector<unsigned char>& buffer){
    int n = buffer[index], len;
    std::stringstream ss;
    const std::string STR_BYTE = std::bitset<8>(n).to_string();
    index++;
    if (STR_BYTE.substr(0, 2) == "00"){
        len = std::stoi(STR_BYTE.substr(2, 6), nullptr, 2);
        while (len > 0){
            ss << buffer[index];
            len --;
            index ++;
        }
        return  ss.str();
    }
    else if (STR_BYTE.substr(0, 2) == "01"){
        n = buffer[index];
        len = std::stoi(STR_BYTE.substr(2, 6) + std::bitset<8>(n).to_string(), nullptr, 2);
        index++;
        while (len > 0){
            ss << buffer[index];
            len --;
            index ++;
        }
        return ss.str();
    }
    else if (STR_BYTE.substr(0, 2) == "10"){
        std::string lenstr = "";
        for (int i = 0; i < 4; i++){
            n = buffer[index]; 
            lenstr += std::bitset<8>(n).to_string();
            index ++;
        }
        len = std::stoi(lenstr, nullptr, 2); 
        while (len > 0){
            ss <<  buffer[index];
            len --;
            index ++;
        }   
        return  ss.str();   
    }
    else {
        if (STR_BYTE == "11000000"){
            n = buffer[index];
            index++;
            return std::to_string(n);            
        } 
        else if (STR_BYTE == "11000001"){
            unsigned char binary_num[] = {buffer[index+1], buffer[index]};
            int num;
            std::memcpy(&num, binary_num, sizeof(int)/2); 
            index += 2;
            return std::to_string(num);           
        }
        else if (STR_BYTE == "11000010"){
            unsigned char binary_num[] = {buffer[index+3], buffer[index+2], buffer[index+1], buffer[index]};
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

void get_key_value_pair(std::vector<unsigned char>& buffer, int &index, std::vector<std::string>& keys, std::vector<std::string>& values){
    if (buffer[index] == 0x00){
        index++;
        keys.push_back(decode_str_length(index, buffer));
        values.push_back(decode_str_length(index, buffer));
    }
    else
        index++;
}

enum byte_space {ONE_BYTE=1, TWO_BYTES=2, FOUR_BYTES=4, EIGHT_BYTES=8};

bool check_key_date_validity(std::vector<unsigned char>& buffer, int &index, byte_space nb_bytes){
    unsigned char binary_num[nb_bytes];
    for (int j = 0; j < nb_bytes; j++)
        binary_num[j] = buffer[index+j+1];
    auto now = std::chrono::system_clock::now();
    // Convert the current time to time since epoch
    auto duration = now.time_since_epoch();
    // Convert duration to milliseconds
    int64_t timestamp;
    if (nb_bytes == FOUR_BYTES){
        std::memcpy(&timestamp, binary_num, sizeof(int32_t));
        return timestamp > std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    }
    else {
        std::memcpy(&timestamp, binary_num, sizeof(int64_t));
        return timestamp > std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(); 
    }
}

void skip_key(std::vector<unsigned char>& buffer, int& ind, byte_space nb){
    ind += nb + 2;
    decode_str_length(ind, buffer);
    decode_str_length(ind, buffer);
}

std::pair<std::vector<std::string>, std::vector<std::string>> get_keys_values_from_file(const std::string& filepath){
    std::ifstream input_file(filepath, std::ios::binary);
    if (std::filesystem::is_directory(filepath) || !input_file.is_open())
        return {};
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(input_file)),
                              std::istreambuf_iterator<char>());
    
    input_file.close();

    std::stringstream ss;
    std::vector<std::string> keys;
    std::vector<std::string> values;
    
    int index = 0, buffer_size = buffer.size();
    while (index < buffer_size && buffer[index] != 0xFE){
        index++;
    }
    while (index < buffer_size && buffer[index] != 0xFB){
        index++;
    }
    index += 3;
    while (index < buffer_size && buffer[index] != 0xFF){
       if (buffer[index] == 0xFD) { 
            if (check_key_date_validity(buffer, index, FOUR_BYTES)){
                index += 5;
                get_key_value_pair(buffer, index, keys, values);
            }
            else 
                skip_key(buffer, index, FOUR_BYTES);
            continue;     
       }
       if (buffer[index] == 0xFC) { 
            if (check_key_date_validity(buffer, index, EIGHT_BYTES)){
                index += 9;
                get_key_value_pair(buffer, index, keys, values);
            }
            else 
                skip_key(buffer, index, EIGHT_BYTES);
            continue;           
       } 
        get_key_value_pair(buffer, index, keys, values);
    }
    
    return std::make_pair(keys, values);
}

std::vector<unsigned char> string_to_binary(const std::string& str){
    
    int len = str.length();
    int str_binary_len = len / 2;
    std::string prefix = "$" + std::to_string(str_binary_len) + "\r\n"; 
    std::vector<unsigned char> bytes(prefix.begin(), prefix.end());
    for (int i = 0; i < len; i+=2){
        bytes.push_back(static_cast<unsigned char>(std::stoi(str.substr(i, 2), nullptr, 16)));
    }
    return bytes;
} 

std::pair<int, std::vector<unsigned char>> read_file_sent(char* buffer_data, const int& size, int& pos){
    std::string str_num;
    pos++;
    while (pos < size && buffer_data[pos] != '\r')
    {
        str_num += buffer_data[pos];
        pos++;
    }
    std::vector<unsigned char> file_content;
    if (pos == size)
        return std::make_pair(0, file_content);
    int file_content_size = std::stoi(str_num);
    if ((pos = pos + 1) == size)
        return std::make_pair(file_content_size, file_content);
    if ((pos = pos + 1) == size)
        return std::make_pair(file_content_size, file_content);        
    for (int i = 0; i < file_content_size; i++){
        file_content.push_back(buffer_data[pos]);
        pos++;
        if (pos == size)
            break;
    }
    return std::make_pair(file_content_size, file_content);
}

