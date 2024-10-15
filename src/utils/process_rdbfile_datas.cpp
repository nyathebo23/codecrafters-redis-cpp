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
#include <map>

using str_nullable = std::variant<std::string, std::nullptr_t>;


bool is_file_empty(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::ate);  // Open the file in "ate" mode to move the cursor to the end
    return file.tellg() == 0;  // tellg() returns the position of the cursor, which will be 0 if the file is empty
}

std::string decode_str_length(int& index, std::vector<unsigned char>& buffer){
    int n = buffer[index], len;
    std::stringstream ss;
    const std::string str_byte = std::bitset<8>(n).to_string();
    index++;
    if (str_byte.substr(0, 2) == "00"){
        len = std::stoi(str_byte.substr(2, 6), nullptr, 2);
        while (len > 0){
            ss << buffer[index];
            len --;
            index ++;
        }
        return  ss.str();
    }
    else if (str_byte.substr(0, 2) == "01"){
        n = buffer[index];
        len = std::stoi(str_byte.substr(2, 6) + std::bitset<8>(n).to_string(), nullptr, 2);
        index++;
        while (len > 0){
            ss << buffer[index];
            len --;
            index ++;
        }
        return ss.str();
    }
    else if (str_byte.substr(0, 2) == "10"){
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
        if (str_byte == "11000000"){
            n = buffer[index];
            index++;
            return std::to_string(n);            
        } 
        else if (str_byte == "11000001"){
            unsigned char binary_num[] = {buffer[index+1], buffer[index]};
            int num;
            std::memcpy(&num, binary_num, sizeof(int)/2); 
            index += 2;
            return std::to_string(num);           
        }
        else if (str_byte == "11000010"){
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

void get_key_value_pair(std::vector<unsigned char>& buffer, int &index, std::vector<std::any>& keys, std::vector<std::any>& values){
    if (buffer[index] == 0x00){
        index++;
        keys.push_back(decode_str_length(index, buffer));
        values.push_back(decode_str_length(index, buffer));
    }
    else
        index++;
}

enum byte_space {one_byte=1, two_bytes=2, four_bytes=4, eight_bytes=8};

bool check_key_date_validity(std::vector<unsigned char>& buffer, int &index, byte_space nb_bytes){
    unsigned char binary_num[nb_bytes];
    for (int j = 0; j < nb_bytes; j++)
        binary_num[j] = buffer[index+j+1];
    auto now = std::chrono::system_clock::now();
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

void skip_key(std::vector<unsigned char>& buffer, int& ind, byte_space nb){
    ind += nb + 2;
    decode_str_length(ind, buffer);
    decode_str_length(ind, buffer);
}

std::pair<std::vector<std::any>, std::vector<std::any>> get_keys_values_from_file(std::string filepath){
    std::ifstream input_file(filepath, std::ios::binary);
    if (std::filesystem::is_directory(filepath) || !input_file.is_open())
        return {};
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(input_file)),
                              std::istreambuf_iterator<char>());
    
    input_file.close();

    std::stringstream ss;
    std::vector<std::any> keys;
    std::vector<std::any> values;
    
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
            if (check_key_date_validity(buffer, index, four_bytes)){
                index += 5;
                get_key_value_pair(buffer, index, keys, values);
            }
            else 
                skip_key(buffer, index, four_bytes);
            continue;     
       }
       if (buffer[index] == 0xFC) { 
            if (check_key_date_validity(buffer, index, eight_bytes)){
                index += 9;
                get_key_value_pair(buffer, index, keys, values);
            }
            else 
                skip_key(buffer, index, eight_bytes);
            continue;           
       } 
        get_key_value_pair(buffer, index, keys, values);
    }
    
    return std::make_pair(keys, values);
}

std::vector<unsigned char> string_to_binary(std::string str){
    
    int len = str.length();
    int str_binary_len = len / 2;
    std::string prefix = "$" + std::to_string(str_binary_len) + "\r\n"; 
    std::vector<unsigned char> bytes(prefix.begin(), prefix.end());
    for (int i = 0; i < len; i+=2){
        bytes.push_back(static_cast<unsigned char>(std::stoi(str.substr(i, 2), nullptr, 16)));
    }
    return bytes;
} 

std::pair<int, std::string> read_file_sent(char* buffer_data, int size){
    
    int pos = 1;
    std::string str_num;
    unsigned char myUnsignedCharArray[size];

    // Conversion de chaque élément
    for (int i = 0; i < size; ++i) {
        myUnsignedCharArray[i] = static_cast<unsigned char>(buffer_data[i]);
    }
    std::cout <<  myUnsignedCharArray[0] << myUnsignedCharArray[1] << myUnsignedCharArray[2];
    while (myUnsignedCharArray[pos] != '\r')
    {
        pos++;
        str_num += buffer_data[pos];
    }
    std::string file;
    return std::make_pair(std::stoi(str_num), file);
    
}


// int main(int argc, char **argv) {
//     std::vector<std::any> str = get_keys_values_from_file("rdbfile.rdb");
//     std::cout << str.size();
//     for (int i = 0; i < str.size(); i++)
//         std::cout << std::any_cast<std::string>(str[i]);
//     return 0;
// }