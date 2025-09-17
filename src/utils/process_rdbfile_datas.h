#ifndef PROCESS_RDBFILE_DATAS_H
#define PROCESS_RDBFILE_DATAS_H

#include <string>
#include <vector>

std::pair<std::vector<std::string>, std::vector<std::string>> get_keys_values_from_file(const std::string& filepath);
std::vector<unsigned char> string_to_binary(const std::string& str);
std::pair<int, std::vector<unsigned char>> read_file_sent(char* buffer_data, const int& size, int& pos);


#endif