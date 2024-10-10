#include <iostream>
#include <string>
#include <cmath>
#include <variant>
#include "simple_data_parser_dec.h"

bool detect_wrong_char_in_substring(const std::string& str){
    if (str.find("\r\n") != std::string::npos)
        return true;
    return false;
};

std::pair<std::string, bool> parse_decode_simple_string(const std::string& msg){
    const std::string str = msg.substr(1, msg.length()-3);
    if (detect_wrong_char_in_substring(str))
        return std::make_pair("", false);
    return std::make_pair(str, true);
}

std::pair<std::string, bool> parse_decode_error_msg(const std::string& msg){
    const std::string str = msg.substr(1, msg.length()-3);
    if (detect_wrong_char_in_substring(str))
        return std::make_pair("", false);
    return std::make_pair(str, true);
}

std::pair<std::variant<int, std::string>, bool> parse_decode_integer(const std::string& msg){
    const std::string strnum = msg.substr(1, msg.length()-3);
    try {
        int number = std::stoi(strnum);
        return std::make_pair(number, true);
    } catch (const std::invalid_argument& e) {
        return std::make_pair(e.what(), false);
    } catch (const std::out_of_range& e) {
        return std::make_pair(e.what(), false);
    }
}

std::pair<std::variant<bool, std::string>, bool> parse_decode_boolean(const std::string& msg){
    if (msg.compare("#t\r\n") == 0)
        return std::make_pair(true, true);
    else if (msg.compare("#f\r\n") == 0)
        return std::make_pair(true, true);
    else return std::make_pair("Error", false);
}

std::pair<std::variant<double, std::string>, bool> parse_decode_double(const std::string& msg){
    std::size_t posE = msg.find('e');
    if (posE == std::string::npos)
        posE = msg.find('E');
    try {
        if (posE == std::string::npos){
            const std::string strnum = msg.substr(1, msg.length() - 3);
            double number = std::stod(strnum);
            return std::make_pair(number, true);
        }
        else {
            const std::string exp = msg.substr(posE + 1, msg.length() - 3 - posE);
            const std::string strnum = msg.substr(1, posE - 1);
            int expint = std::stoi(exp);
            double number = std::stod(strnum) * std::pow(10, expint);
            return std::make_pair(number, true);
        }
    } catch (const std::invalid_argument& e) {
        return std::make_pair("Error", false);
    } catch (const std::out_of_range& e) {
        return std::make_pair("Error", false);
    }
}

bool is_digits(const std::string& str) {
    for (char ch : str) {
        int v = ch; // ASCII Val converted
        if (!(ch >= 48 && ch <= 57)) {
            return false;
        }
    }
    return true;
}
std::pair<std::string, bool> parse_decode_big_number(const std::string& msg){
    const std::string num = msg.substr(1, msg.length() - 3);
    if (detect_wrong_char_in_substring(num) || !is_digits(num))
        return std::make_pair("Error", false);
    return std::make_pair(num, true); 
}

