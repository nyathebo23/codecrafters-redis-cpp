#ifndef PROCESS_RDBFILE_DATAS_H
#define PROCESS_RDBFILE_DATAS_H

#include <string>
#include <vector>
#include <any>

std::string hexstr_to_ASCII_string(std::string hexnums);
std::vector<std::any> get_keys_values_from_file(std::string filepath);

#endif