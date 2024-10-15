#ifndef GLOBAL_DATAS_H
#define GLOBAL_DATAS_H

#include <string>
#include <map>

class GlobalDatas {


    public:
        static bool isMaster;

        static unsigned int prec_commands_offset;

        static bool isRequestFromMaster;

        static unsigned int commands_offset;

        static void set_commands_offset(std::string str);

        static std::map<std::string, std::string> dict_table;

        static void set(std::string key, std::string value);

        static std::string get(std::string key);
};

#endif