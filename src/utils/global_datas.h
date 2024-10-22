#ifndef GLOBAL_DATAS_H
#define GLOBAL_DATAS_H

#include <string>
#include <map>
#include <vector>

using VectorMapEntries = std::vector<std::map<std::string, std::string>>;

class GlobalDatas {
    public:
        static bool isMaster;

        static std::vector<std::pair<std::string, VectorMapEntries>> entries;

        static std::vector<std::pair<std::string, VectorMapEntries>> get_entries();

        static void set_entry(std::vector<std::string> vals);

        static VectorMapEntries get_vector_map_entries_by_key(std::string key);

        static int get_entry_index(std::string entry_key);

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