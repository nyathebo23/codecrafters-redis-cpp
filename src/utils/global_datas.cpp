#include "global_datas.h"
#include <iostream>

bool GlobalDatas::isMaster = true;

bool GlobalDatas::isRequestFromMaster = false;

unsigned int GlobalDatas::prec_commands_offset = 0;

unsigned int GlobalDatas::commands_offset = 0;

void GlobalDatas::set_commands_offset(std::string str){
    GlobalDatas::prec_commands_offset = GlobalDatas::commands_offset;
    GlobalDatas::commands_offset += str.size();
    std::cout << commands_offset << "  " << prec_commands_offset;
};

std::map<std::string, std::string> GlobalDatas::dict_table;

void GlobalDatas::set(std::string key, std::string value){
    dict_table[key] = value;
}
std::string GlobalDatas::get(std::string key){
    return dict_table[key];
}
