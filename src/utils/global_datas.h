#ifndef GLOBAL_DATAS_H
#define GLOBAL_DATAS_H

#include <string>
#include <map>

class GlobalDatas {


    public:
        static bool isMaster;

        static bool isRequestFromMaster;

        static std::map<std::string, std::string> dict_table;

        static void set(std::string key, std::string value);

        static std::string get(std::string key);
};

#endif