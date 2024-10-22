#ifndef STREAM_COMMANDS_PROCESSING_H
#define STREAM_COMMANDS_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include <any>


class StreamCommandsProcessing {

    public:
        static std::string xadd(std::vector<std::string> extras);
        static std::string xrange(std::vector<std::string> extras);
        static std::string xread(std::vector<std::string> extras);
        static std::string xread_with_block(std::vector<std::string> extras);
        static std::pair<std::string, std::string> split_entry_id(std::string str);
        static std::pair<unsigned long, unsigned int> split_entry_id_num(std::string str);

};

#endif