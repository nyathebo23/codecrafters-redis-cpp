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

std::string get_key_from_line(std::ifstream& file, std::string& line){
    if (line == "00") {
        std::cout << "azerjkj";
        getline(file, line);
        const std::string key = hexstr_to_ASCII_string(line);
        return key;
    }
    return "";
}

std::vector<std::any> get_keys_values_from_file(std::string filepath){
    std::ifstream input_file(filepath);
    std::vector<std::any> keys;
    if (!input_file.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
      return {};
    }
    std::string line;
    while (line != "FE") {
      getline(input_file, line);
    }
    while (line != "FB") {
      getline(input_file, line);
    }
    getline(input_file, line);
    getline(input_file, line);

    while(line != "FF"){
      if (line == "FC") {
        getline(input_file, line);
        getline(input_file, line);
        keys.push_back(get_key_from_line(input_file, line));
        continue;
      }
      if (line == "FD") {
        getline(input_file, line);
        getline(input_file, line);
        keys.push_back(get_key_from_line(input_file, line));
        continue;
      } 
      if (line == "00")
        keys.push_back(get_key_from_line(input_file, line));
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
//     std::vector<std::string> str = get_keys_values_from_file("rdbfile.txt");
//     std::cout << str.size();
//     for (int i = 0; i < str.size(); i++)
//         std::cout << str[i];

//     return 0;
// }