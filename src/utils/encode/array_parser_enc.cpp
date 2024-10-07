#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>
#include <cmath>
#include "array_parser_enc.h"
#include "simple_data_parser_enc.h"
#include "small_aggregate_parser_enc.h"


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