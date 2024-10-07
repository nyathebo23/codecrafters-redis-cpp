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


using str_nullable = std::variant<std::string, std::nullptr_t>;

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return ""; // If the string is all spaces, return an empty string
    }
    
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

str_nullable get_key_from_line(std::ifstream& file, std::string& line){
    if (line == "00") {
        getline(file, line);
        line = trim(line);
        const std::string key = hexstr_to_ASCII_string(line);
        return key;
    }
    return nullptr;
}

bool is_file_empty(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::ate);  // Open the file in "ate" mode to move the cursor to the end
    return file.tellg() == 0;  // tellg() returns the position of the cursor, which will be 0 if the file is empty
}

std::vector<std::any> get_keys_values_from_file(std::string filepath){
    if (is_file_empty(filepath))
        return {};
    std::ifstream input_file(filepath);
    if (!input_file.is_open())
      return {};

    std::vector<std::any> keys;
    std::string line;
    while (trim(line) != "FE" && !input_file.eof()) {
      getline(input_file, line);
    }
    while (trim(line) != "FB" && !input_file.eof()) {
      getline(input_file, line);
    }
    getline(input_file, line);
    getline(input_file, line);

    auto add_key = [&input_file, &line, &keys]() {
        getline(input_file, line);
        line = trim(line);
        try {
            const std::string key = std::get<std::string>(get_key_from_line(input_file, line));
            keys.push_back(key);
        }
        catch(const std::exception& e){}
    };

    while(trim(line) != "FF"  && !input_file.eof()){
      if (trim(line) == "FC") {
        getline(input_file, line);
        add_key();
        continue;
      }
      if (trim(line) == "FD") {
        getline(input_file, line);
        add_key();
        continue;
      } 
      if ((line = trim(line)) == "00"){
        try {
            const std::string key = std::get<std::string>(get_key_from_line(input_file, line));
            keys.push_back(key);
        }
        catch(const std::exception& e){}
      }
      getline(input_file, line);
    }
    return keys;
}

std::string hexstr_to_ASCII_string(std::string hexnums) {        
    std::stringstream ss;
    std::istringstream iss(hexnums);
    std::string hexnum;
    int jump_to = 0;
    iss >> hexnum;
    unsigned int n;
    std::stringstream hexcharstream;
    hexcharstream << std::hex << hexnum[0];
    hexcharstream >> n;
    const std::string semibyte = std::bitset<4>(n).to_string();
    if (semibyte.substr(0, 2) == "01"){
        jump_to = 1;
    }
    else if (semibyte.substr(0, 2) == "10"){
        jump_to = 4;
    }
    else if (semibyte.substr(0, 2) == "11"){
        if (hexnum == "C0"){
            iss >> hexnum;
            unsigned int num = std::stoi(hexnum, nullptr, 16); 
            return std::to_string(num);
        } else if (hexnum == "C1"){
            std::string num = "";
            for (int i = 1; i >= 0; i--){
                iss >> hexnum;
                num = hexnum + num;
            }
            return std::to_string(std::stoi(num, nullptr, 16));
        } else if (hexnum == "C2") {
            std::string num = "";
            for (int i = 3; i >= 0; i--){
                iss >> hexnum;
                num = hexnum + num;
            }
            return std::to_string(std::stoi(num, nullptr, 16));
        }
    }  
    while (jump_to > 0){
        iss >> hexnum;
        jump_to--;
    }
    while (iss >> hexnum) {
      unsigned char byte = stoi(hexnum, nullptr, 16);
      ss << byte;    
    }
    return ss.str();
}

// int main(int argc, char **argv) {
//     std::vector<std::any> str = get_keys_values_from_file("rdbfile.rdb");
//     std::cout << str.size();
//     for (int i = 0; i < str.size(); i++)
//         std::cout << std::any_cast<std::string>(str[i]);

//     return 0;
// }