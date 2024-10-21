#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>
#include <cmath>
#include "array_parser_enc.h"
#include "simple_data_parser_enc.h"
#include "small_aggregate_parser_enc.h"
#include "../global_datas.h"

std::string parse_encode_array(const std::vector<std::any>& msg){
    if (msg.size() == 0)
        return "*0\r\n";
    const std::string startenc = "*" + std::to_string(msg.size()) + "\r\n";
    std::string encode_text = ""; 
    for (std::any token: msg){
        if (token.type() == typeid(int)){
            encode_text += parse_encode_integer(std::any_cast<int>(token));
        } else if (token.type() == typeid(double)){
            encode_text += parse_encode_double(std::any_cast<double>(token));
        } else if (token.type() == typeid(std::string)) {
            encode_text += parse_encode_bulk_string(std::any_cast<std::string>(token));
        } else if (token.type() == typeid(bool)) {
            encode_text += parse_encode_boolean(std::any_cast<bool>(token));
        } 
    }
    return startenc + encode_text; 
}

std::string parse_encode_array_for_xrange(const VectorMapEntries data){
    int vector_map_size = data.size();
    if (vector_map_size == 0)
        return "*0\r\n";
    VectorMapEntries vmap;
    std::string entry_encoded = "*" + std::to_string(vector_map_size) +  "\r\n";
    for (auto map_entry: data){
        entry_encoded += "*2\r\n";
        entry_encoded += parse_encode_bulk_string(map_entry["id"]);
        std::vector<std::any> keys_vals_list;
        std::map<std::string, std::string>::iterator it = map_entry.begin();
        while (it != map_entry.end()){
            if (it->first != "id"){
                keys_vals_list.push_back(it->first);
                keys_vals_list.push_back(it->second);
            }
            ++it;
        }
        entry_encoded += parse_encode_array(keys_vals_list);
    }
    return entry_encoded; 
}

std::string parse_encode_array_for_xread(const std::vector<std::pair<std::string, VectorMapEntries>> data){
    int vector_size = data.size();
    if (vector_size == 0)
        return "$-1\r\n";
    std::string entry_encoded = "*" + std::to_string(vector_size) + "\r\n";
    for (auto key_vector_map: data) {
        entry_encoded += "*2\r\n";
        entry_encoded += parse_encode_bulk_string(key_vector_map.first);
        entry_encoded += "*" + std::to_string(key_vector_map.second.size()) + "\r\n";
        for (auto map_entry: key_vector_map.second) {
            entry_encoded += "*2\r\n";
            entry_encoded += parse_encode_bulk_string(map_entry["id"]);
            std::vector<std::any> keys_vals_list;
            std::map<std::string, std::string>::iterator it = map_entry.begin();
            while (it != map_entry.end()){
                if (it->first != "id"){
                    keys_vals_list.push_back(it->first);
                    keys_vals_list.push_back(it->second);
                }
                ++it;
            }
            entry_encoded += parse_encode_array(keys_vals_list);
        }
    }
    std::cout << entry_encoded;
    return entry_encoded; 
}