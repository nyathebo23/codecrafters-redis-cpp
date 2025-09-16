#include "global_datas.h"
#include <iostream>


bool GlobalDatas::isMaster = true;
CommandsOffset GlobalDatas::cmdsOffset = CommandsOffset();
StreamList GlobalDatas::streamList = StreamList();
RDBDatasDict GlobalDatas::datasDict = RDBDatasDict();
Lists GlobalDatas::lists = Lists();
Channels GlobalDatas::channels = Channels();
SortedSets GlobalDatas::sortedSets = SortedSets();

std::map<int, int> GlobalMasterDatas::replicas_offsets;
unsigned int GlobalMasterDatas::prec_commands_offset = 0;
unsigned int GlobalMasterDatas::commands_offset = 0;
void GlobalMasterDatas::set_commands_offset(unsigned short offset, bool is_write_cmd){
    if (is_write_cmd){
        GlobalMasterDatas::commands_offset += offset;
        GlobalMasterDatas::prec_commands_offset = GlobalMasterDatas::commands_offset;
    }
    else {
        GlobalMasterDatas::prec_commands_offset = GlobalMasterDatas::commands_offset;
        GlobalMasterDatas::commands_offset += offset;        
    }
};
