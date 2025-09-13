#include <iostream>
#include <string>
#include <cmath>
#include "small_aggregate_parser_dec.h"

std::string encError = "Error in encoding";
std::string badTextLength = "Text Length is incorrect";
std::string nanTextLength = "Text length is not a number";
std::string outRangeTextLength = "Text length out of range";

StringDecodeResult parse_decode_bulk_string(const std::string& msg){
    if (msg.substr(1, 2).compare("-1") == 0)
        if (msg.compare("$-1\r\n") != 0)
            return StringDecodeResult(encError);
        else
            return StringDecodeResult("", std::nullopt);

    const auto msglen = msg.length();
    const unsigned int start_symbol_pos = msg.find("\r\n");
    const unsigned int end_symbol_pos = msg.find("\r\n", start_symbol_pos + 2); 
    if (start_symbol_pos == msglen - 2 || end_symbol_pos != msglen - 2)
            return StringDecodeResult(encError);

    const std::string strnum =  msg.substr(1, start_symbol_pos - 1);
    const std::string text = msg.substr(start_symbol_pos + 2, end_symbol_pos - start_symbol_pos - 2);

    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return StringDecodeResult(badTextLength);
        else
            return StringDecodeResult(text, std::nullopt);
    } catch (const std::invalid_argument& e) {
            return StringDecodeResult(nanTextLength);
    } catch (const std::out_of_range& e) {
            return StringDecodeResult(outRangeTextLength);
    }
}

StringDecodeResult  parse_decode_bulk_error(const std::string& msg){

    const auto msglen = msg.length();
    const unsigned int start_symbol_pos = msg.find("\r\n");
    const unsigned int end_symbol_pos = msg.find("\r\n", start_symbol_pos + 2); 
    if (start_symbol_pos == msglen - 2 || end_symbol_pos != msglen - 2)
         return StringDecodeResult(encError);

    const std::string strnum =  msg.substr(1, start_symbol_pos - 1);
    const std:: string text = msg.substr(start_symbol_pos + 2, end_symbol_pos - start_symbol_pos - 2);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return StringDecodeResult(badTextLength);
        else
            return StringDecodeResult(text, std::nullopt);
    } catch (const std::invalid_argument& e) {
        return StringDecodeResult(nanTextLength);
    } catch (const std::out_of_range& e) {
        return StringDecodeResult(outRangeTextLength);
    }
}


StringDecodeResult  parse_decode_verbatim_string(const std::string& msg){

    const auto msglen = msg.length();
    const unsigned int start_symbol_pos = msg.find("\r\n");
    const unsigned int end_symbol_pos = msg.find("\r\n", start_symbol_pos + 2); 
    if (start_symbol_pos == msglen - 2 || end_symbol_pos != msglen - 2)
         return StringDecodeResult(encError);

    const std::string strnum =  msg.substr(1, start_symbol_pos - 1);
    const std:: string text = msg.substr(start_symbol_pos + 2, end_symbol_pos - start_symbol_pos - 2);

    const int pos_separator = text.find(':');
    const std::string encoding = text.substr(0, pos_separator);
    if (encoding.length() != 3 && pos_separator != std::string::npos)
        return StringDecodeResult(encError);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return StringDecodeResult(badTextLength);
        else
            return StringDecodeResult(text, std::nullopt);
    } catch (const std::invalid_argument& e) {
        return StringDecodeResult(nanTextLength);
    } catch (const std::out_of_range& e) {
        return StringDecodeResult(outRangeTextLength);
    }
}

