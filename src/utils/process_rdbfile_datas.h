#ifndef PROCESS_RDBFILE_DATAS_H
#define PROCESS_RDBFILE_DATAS_H

#include <string>
#include <vector>
#include <any>

std::pair<std::vector<std::any>, std::vector<std::any>> get_keys_values_from_file(std::string filepath);
std::vector<unsigned char> string_to_binary(std::string str);

#endif