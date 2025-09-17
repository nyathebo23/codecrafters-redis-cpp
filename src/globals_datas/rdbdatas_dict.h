#ifndef RDB_DATAS_DICT_H
#define RDB_DATAS_DICT_H

#include <string>
#include <map>

class RDBDatasDict {
    public:
        void set(const std::string& key, const std::string& value);
        void erase(const std::string& key);
        bool exist(const std::string& key);
        std::string get(const std::string& key);

    private:
        std::map<std::string, std::string> datas_dict;

};

#endif