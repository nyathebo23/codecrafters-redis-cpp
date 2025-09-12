#include "stream.h"

void Stream::add_entry(std::map<std::string, std::string> map_entry) {
    this->entries.push_back(map_entry);
}

std::string Stream::get_key() {
    return this->key;
}

std::map<std::string, std::string> Stream::last_entry() {
    return entries.back();
};

Stream::Stream(std::string key, std::map<std::string, std::string> map_entry) {
    this->key = key;
    this->entries.push_back(map_entry);
}

VectorMapEntries Stream::get_entries() {
    return this->entries;
};

void StreamList::set_entry(std::vector<std::string> args){
    std::map<std::string, std::string> map_entry;
    map_entry["id"] = args[1];
    for (int i = 2; i < args.size()-1; i+=2)
        map_entry[args[i]] = args[i+1];
    int index = this->get_stream_index(args[0]);
    if (index == this->streams.size()){
        Stream stream(args[0], map_entry);
        this->streams.push_back(stream);
    }
    else {
        this->streams[index].add_entry(map_entry);
    }        
};


std::vector<Stream> StreamList::get_stream_list() {
    return this->streams;
}

int StreamList::get_stream_index(std::string stream_key) {
    int index = 0, size = streams.size();
    while (index < size && streams[index].get_key() != stream_key){
        index++;
    } 
    return index;
}

std::size_t StreamList::size() {
    return this->streams.size();
}

bool StreamList::exist(std::string stream_key) {
    return this->get_stream_index(stream_key) != this->streams.size();
};

Stream StreamList::get_by_index(size_t index) {
    return this->streams[index];
};