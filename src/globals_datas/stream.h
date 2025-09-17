#ifndef STREAM_DATAS_H
#define STREAM_DATAS_H

#include <string>
#include <map>
#include <vector>

using VectorMapEntries = std::vector<std::map<std::string, std::string>>;

class Stream {
    public:
        void add_entry(const std::map<std::string, std::string>& map_entry);
        std::map<std::string, std::string> last_entry();
        std::string get_key();
        VectorMapEntries get_entries();
        Stream(const std::string& key, const std::map<std::string, std::string>& map_entry);

    private:
        std::string key;
        VectorMapEntries entries;

};

class StreamList {
    private:
        std::vector<Stream> streams;

    public:
        void set_entry(const std::vector<std::string>& args);
        int get_stream_index(const std::string& stream_key);
        Stream get_by_index(const size_t& index);
        std::size_t size();
        bool exist(const std::string& stream_key);
        std::vector<Stream> get_stream_list(); 
    
};

#endif