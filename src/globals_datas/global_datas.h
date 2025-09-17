#ifndef GLOBAL_DATAS_H
#define GLOBAL_DATAS_H

#include <string>
#include <map>
#include <vector>
#include "rdbdatas_dict.h"
#include "commands_offset.h"
#include "lists.h"
#include "stream.h"
#include "channels.h"
#include "sorted_sets.h"

using VectorMapEntries = std::vector<std::map<std::string, std::string>>;

class GlobalDatas {
    public:
        static bool is_master;
        static StreamList stream_list;
        static CommandsOffset cmds_offset;
        static RDBDatasDict datas_dict;
        static Channels channels;
        static Lists lists;
        static SortedSets sorted_sets;
};

class GlobalMasterDatas {
    public:
        static unsigned int prec_commands_offset;
        static unsigned int commands_offset;
        static std::map<int, int> replicas_offsets;  
        static void set_commands_offset(unsigned short offset, bool is_write_cmd);
};

#endif