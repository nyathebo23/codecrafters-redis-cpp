#ifndef STREAM_COMMANDS_PROCESSING_H
#define STREAM_COMMANDS_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include <any>


class StreamCommandsProcessing {

    public:

        static void xadd(std::vector<std::string> extras, int dest_fd);
        static void xrange(std::vector<std::string> extras, int dest_fd);
        static void xread(std::vector<std::string> extras, int dest_fd);
        static std::pair<std::string, std::string> split_entry_id(std::string str);
        static std::pair<unsigned long, unsigned int> split_entry_id_num(std::string str);

};

#endif