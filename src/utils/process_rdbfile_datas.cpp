#include <iostream>
#include <string>
#include <cmath>
#include <variant>
#include <fstream>
#include <vector>
#include <any>
#include <sstream>
#include <bitset>
#include "process_rdbfile_datas.h"
#include <cstring> 
#include <algorithm>

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
        return ss.str();
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
            index ++;
            ss << (unsigned char) buffer[index];
            len --;
        }   
        return "azertt";    
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

void get_key_name(std::vector<char>& buffer, int &index, std::vector<std::any>& keys){
    if ((int)(unsigned char)buffer[index] == 0){
        keys.push_back(decode_str_length(index, buffer));
    }
}


std::vector<std::any> get_keys_values_from_file(std::string filepath){
    if (is_file_empty(filepath))
        return {};
    std::ifstream input_file(filepath, std::ios::binary);
    std::stringstream ss;
    if (!input_file.is_open())
        return {};
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
    index++;
    while (index < buffer_size && (int)(unsigned char)buffer[index] != 255){
       index++;
       if ((int)(unsigned char)buffer[index] == 253) { 
            index += 5;
            if ((int)(unsigned char)buffer[index] == 0){
                keys.push_back(decode_str_length(index, buffer));
            } 
            continue;     
       }
       if ((int)(unsigned char)buffer[index] == 252) { 
            index += 9;
            if ((int)(unsigned char)buffer[index] == 0){
                keys.push_back(decode_str_length(index, buffer));
            } 
            continue;           
       } 
        if ((int)(unsigned char)buffer[index] == 0){
            index++;
            keys.push_back(decode_str_length(index, buffer));
        }      
    }
    return keys;

}


// int main(int argc, char **argv) {
//     std::vector<std::any> str = get_keys_values_from_file("rdbfile.rdb");
//     std::cout << str.size();
//     for (int i = 0; i < str.size(); i++)
//         std::cout << std::any_cast<std::string>(str[i]);
//     return 0;
// }