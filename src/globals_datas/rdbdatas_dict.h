#ifndef RDB_DATAS_DICT_H
#define RDB_DATAS_DICT_H

#include <string>
#include <map>

class RDBDatasDict {
    public:
        void set(std::string key, std::string value);
        void erase(std::string key);
        bool exist(std::string key);
        std::string get(std::string key);

    private:
        std::map<std::string, std::string> datas_dict;

};

#endif