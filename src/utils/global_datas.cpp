#include "global_datas.h"
#include <iostream>

bool GlobalDatas::isMaster = true;

unsigned int GlobalDatas::prec_commands_offset = 0;

unsigned int GlobalDatas::commands_offset = 0;

void GlobalDatas::set_commands_offset(unsigned short offset){
    GlobalDatas::prec_commands_offset = GlobalDatas::commands_offset;
    GlobalDatas::commands_offset += offset;
};

std::map<std::string, std::string> GlobalDatas::dict_table;

void GlobalDatas::set(std::string key, std::string value){
    dict_table[key] = value;
}
std::string GlobalDatas::get(std::string key){
    return dict_table[key];
}



std::map<int, int> GlobalMasterDatas::replicas_offsets;

unsigned int GlobalMasterDatas::prec_commands_offset = 0;

unsigned int GlobalMasterDatas::commands_offset = 0;

void GlobalMasterDatas::set_commands_offset(unsigned short offset, bool is_write_cmd){
    if (is_write_cmd){
        GlobalMasterDatas::prec_commands_offset += offset;
        GlobalMasterDatas::commands_offset = GlobalMasterDatas::prec_commands_offset;
    }
    else {
        GlobalMasterDatas::prec_commands_offset = GlobalMasterDatas::commands_offset;
        GlobalMasterDatas::commands_offset += offset;        
    }
};
