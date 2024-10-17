#ifndef GLOBAL_DATAS_H
#define GLOBAL_DATAS_H

#include <string>
#include <map>
#include <vector>

class GlobalDatas {
    public:
        static bool isMaster;

        static std::vector<std::map<std::string, std::string>> entries;

        static void set_entry(std::vector<std::string> vals);

        static unsigned int prec_commands_offset;

        static unsigned int commands_offset;

        static void set_commands_offset(unsigned short offset);

        static std::map<std::string, std::string> dict_table;

        static void set(std::string key, std::string value);

        static std::string get(std::string key);
};

class GlobalMasterDatas {

    public:
        static unsigned int prec_commands_offset;

        static unsigned int commands_offset;

        static std::map<int, int> replicas_offsets;  

        static void set_commands_offset(unsigned short offset, bool is_write_cmd);

};

#endif