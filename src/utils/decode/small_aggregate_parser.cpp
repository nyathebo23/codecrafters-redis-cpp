#include <iostream>
#include <string>
#include <cmath>
#include <variant>
#include "small_aggregate_parser.h"
#include "simple_data_parser.h"



std::pair<std::string, bool> parse_decode_bulk_string(const std::string& msg){

    if (msg.substr(1, 2).compare("-1") == 0)
        if (msg.compare("$-1\r\n") != 0)
            return std::make_pair("Error", false);
        else
            return std::make_pair("", true);

    const std::size_t endPosr = msg.find('\r');
    const std::size_t endPosr2 = msg.find('\r', endPosr+1);

    if (endPosr == std::string::npos || endPosr2 == std::string::npos || msg[endPosr+1] != '\n' || 
        msg[endPosr2+1] != '\n' || endPosr2 + 2 != msg.length()){
        return std::make_pair("Error", false);
    }
    const std::string strnum =  msg.substr(1, endPosr - 1);
    const std:: string text = msg.substr(endPosr+2, endPosr2 - endPosr - 2);

    if (detect_wrong_char_in_substring(text))
        return std::make_pair("", false);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return std::make_pair(text, false);
        else
            return std::make_pair(text, true);
    } catch (const std::invalid_argument& e) {
            return std::make_pair("Text length error", false);
    } catch (const std::out_of_range& e) {
            return std::make_pair("Text length out of range", false);
    }
}

std::pair<std::string, bool> parse_decode_bulk_error(const std::string& msg){

    const std::size_t endPosr = msg.find('\r');
    const std::size_t endPosr2 = msg.find('\r', endPosr+1);

    if (endPosr == std::string::npos || endPosr2 == std::string::npos || msg[endPosr+1] != '\n' || 
        msg[endPosr2+1] != '\n' || endPosr2 + 2 != msg.length()){
        return std::make_pair("Error", false);
    }
    const std::string strnum =  msg.substr(1, endPosr - 1);
    const std:: string text = msg.substr(endPosr+2, endPosr2 - endPosr - 2);
    if (detect_wrong_char_in_substring(text))
        return std::make_pair("", false);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return std::make_pair(text, false);
        else
            return std::make_pair(text, true);
    } catch (const std::invalid_argument& e) {
        return std::make_pair("Error", false);
    } catch (const std::out_of_range& e) {
        return std::make_pair("Error", false);
    }
}


std::pair<std::string, bool> parse_decode_verbatim_string(const std::string& msg){

    const std::size_t endPosr = msg.find('\r');
    const std::size_t endPosr2 = msg.find('\r', endPosr+1);
    if (endPosr == std::string::npos || endPosr2 == std::string::npos || msg[endPosr+1] != '\n' || 
        msg[endPosr2+1] != '\n' || endPosr2 + 2 != msg.length()){
        return std::make_pair("Error", false);
    }

    const std::string strnum =  msg.substr(1, endPosr - 1);
    const std:: string text = msg.substr(endPosr+2, endPosr2 - endPosr - 2);
    
    if (detect_wrong_char_in_substring(text))
        return std::make_pair("", false);
    const int pos_separator = text.find(':');
    const std::string encoding = text.substr(0, pos_separator);
    if (encoding.length() != 3 && pos_separator != std::string::npos)
        return std::make_pair("Format encoding Error", false);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return std::make_pair(text, false);
        else
            return std::make_pair(text, true);
    } catch (const std::invalid_argument& e) {
        return std::make_pair("Text length error", false);
    } catch (const std::out_of_range& e) {
        return std::make_pair("Text length out of range", false);
    }
}

