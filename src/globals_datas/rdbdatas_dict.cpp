#include "rdbdatas_dict.h"

void RDBDatasDict::set(const std::string& key, const std::string& value) {
    this->datas_dict[key] = value;
}

std::string RDBDatasDict::get(const std::string& key) {
    return this->datas_dict[key];
};

void RDBDatasDict::erase(const std::string& key) {
    this->datas_dict.erase(key);
}

bool RDBDatasDict::exist(const std::string& key) {
    return this->datas_dict.count(key) != 0;
}