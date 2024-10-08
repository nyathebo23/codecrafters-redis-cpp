#ifndef PROCESS_RDBFILE_DATAS_H
#define PROCESS_RDBFILE_DATAS_H

#include <string>
#include <vector>
#include <any>

void get_key_name(std::vector<char>&, int&, std::vector<std::any>&);
std::vector<std::any> get_keys_values_from_file(std::string filepath);
std::string decode_str_length(int&, std::vector<char>&);

#endif