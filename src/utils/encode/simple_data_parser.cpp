#include <iostream>
#include <string>
#include <cmath>
#include <variant>
#include "simple_data_parser.h"


std::string parse_encode_simple_string(const std::string& msg){
    return "+" + msg + "\r\n";
}

std::string parse_encode_error_msg(const std::string& msg){
    return "-" + msg + "\r\n";
}

std::string parse_encode_integer(const int& msg){
    return ":" + std::to_string(msg) + "\r\n"; 
}

std::string parse_encode_boolean(const bool& msg){
    return msg ? "#t\r\n" : "#f\r\n";
}

std::string parse_encode_double(const double& msg){
    return "," + std::to_string(msg) + "\r\n";
}

std::string parse_encode_big_number(const std::string& msg){
    return "(" + msg + "\r\n";
}

