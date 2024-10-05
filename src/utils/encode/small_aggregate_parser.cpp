#include <iostream>
#include <string>
#include <cmath>
#include <variant>
#include "small_aggregate_parser.h"


std::string parse_encode_bulk_string(const std::string& msg){
    return "$" + std::to_string(msg.length()) + "\r\n" + msg + "\r\n";
}

std::string parse_encode_bulk_error(const std::string& msg){
    return "!" + std::to_string(msg.length()) + "\r\n" + msg + "\r\n";
}

std::string parse_encode_verbatim_string(const std::string& msg, const std::string& encoding){
    return "=" + std::to_string(msg.length()) + "\r\n" + encoding + ':' + msg + "\r\n";
}