#include "rdbdatas_dict.h"

void RDBDatasDict::set(std::string key, std::string value) {
    this->datas_dict[key] = value;
}

std::string RDBDatasDict::get(std::string key) {
    return this->datas_dict[key];
};

void RDBDatasDict::erase(std::string key) {
    this->datas_dict.erase(key);
}

bool RDBDatasDict::exist(std::string key) {
    return this->datas_dict.count(key) != 0;
}