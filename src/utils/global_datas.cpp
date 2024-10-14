#include "global_datas.h"


bool GlobalDatas::isMaster = true;

bool GlobalDatas::isRequestFromMaster = false;

std::map<std::string, std::string> GlobalDatas::dict_table;

void GlobalDatas::set(std::string key, std::string value){
    dict_table[key] = value;
}
std::string GlobalDatas::get(std::string key){
    return dict_table[key];
}
