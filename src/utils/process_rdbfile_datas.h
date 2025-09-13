#ifndef PROCESS_RDBFILE_DATAS_H
#define PROCESS_RDBFILE_DATAS_H

#include <string>
#include <vector>
#include <any>

std::pair<std::vector<std::string>, std::vector<std::string>> get_keys_values_from_file(std::string filepath);
std::vector<unsigned char> string_to_binary(std::string str);
std::pair<int, std::vector<unsigned char>> read_file_sent(char* buffer_data, int size, int& pos);


#endif