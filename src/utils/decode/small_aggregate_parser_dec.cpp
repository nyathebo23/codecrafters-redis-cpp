#include <iostream>
#include <string>
#include <cmath>
#include "small_aggregate_parser_dec.h"

std::string enc_error = "Error in encoding";
std::string bad_text_length = "Text Length is incorrect";
std::string nan_text_length = "Text length is not a number";
std::string out_range_text_length = "Text length out of range";

StringDecodeResult* parse_decode_bulk_string(const std::string& msg){
    if (msg.substr(1, 2).compare("-1") == 0)
        if (msg.compare("$-1\r\n") != 0)
            return new StringDecodeResult(enc_error);
        else
            return new StringDecodeResult("", std::nullopt);

    const auto MSGLEN = msg.length();
    const unsigned int START_SYMBOL_POS = msg.find("\r\n");
    const unsigned int END_SYMBOL_POS = msg.find("\r\n", START_SYMBOL_POS + 2); 
    if (START_SYMBOL_POS == MSGLEN - 2 || END_SYMBOL_POS != MSGLEN - 2)
            return new StringDecodeResult(enc_error);

    const std::string STRNUM =  msg.substr(1, START_SYMBOL_POS - 1);
    const std::string TEXT = msg.substr(START_SYMBOL_POS + 2, END_SYMBOL_POS - START_SYMBOL_POS - 2);

    try {
        int number = std::stoi(STRNUM);
        if (number != TEXT.length())
            return new StringDecodeResult(bad_text_length);
        else
            return new StringDecodeResult(TEXT, std::nullopt);
    } catch (const std::invalid_argument& e) {
            return new StringDecodeResult(nan_text_length);
    } catch (const std::out_of_range& e) {
            return new StringDecodeResult(out_range_text_length);
    }
}

StringDecodeResult* parse_decode_bulk_error(const std::string& msg){

    const auto MSGLEN = msg.length();
    const unsigned int START_SYMBOL_POS = msg.find("\r\n");
    const unsigned int END_SYMBOL_POS = msg.find("\r\n", START_SYMBOL_POS + 2); 
    if (START_SYMBOL_POS == MSGLEN - 2 || END_SYMBOL_POS != MSGLEN - 2)
         return new StringDecodeResult(enc_error);

    const std::string STRNUM =  msg.substr(1, START_SYMBOL_POS - 1);
    const std:: string TEXT = msg.substr(START_SYMBOL_POS + 2, END_SYMBOL_POS - START_SYMBOL_POS - 2);
    try {
        int number = std::stoi(STRNUM);
        if (number != TEXT.length())
            return new StringDecodeResult(bad_text_length);
        else
            return new StringDecodeResult(TEXT, std::nullopt);
    } catch (const std::invalid_argument& e) {
        return new StringDecodeResult(nan_text_length);
    } catch (const std::out_of_range& e) {
        return new StringDecodeResult(out_range_text_length);
    }
}


StringDecodeResult*  parse_decode_verbatim_string(const std::string& msg){

    const auto MSGLEN = msg.length();
    const unsigned int START_SYMBOL_POS = msg.find("\r\n");
    const unsigned int END_SYMBOL_POS = msg.find("\r\n", START_SYMBOL_POS + 2); 
    if (START_SYMBOL_POS == MSGLEN - 2 || END_SYMBOL_POS != MSGLEN - 2)
         return new StringDecodeResult(enc_error);

    const std::string STRNUM =  msg.substr(1, START_SYMBOL_POS - 1);
    const std:: string TEXT = msg.substr(START_SYMBOL_POS + 2, END_SYMBOL_POS - START_SYMBOL_POS - 2);

    const int POS_SEPARATOR = TEXT.find(':');
    const std::string ENCODING = TEXT.substr(0, POS_SEPARATOR);
    if (ENCODING.length() != 3 && POS_SEPARATOR != std::string::npos)
        return new StringDecodeResult(enc_error);
    try {
        int number = std::stoi(STRNUM);
        if (number != TEXT.length())
            return new StringDecodeResult(bad_text_length);
        else
            return new StringDecodeResult(TEXT, std::nullopt);
    } catch (const std::invalid_argument& e) {
        return new StringDecodeResult(nan_text_length);
    } catch (const std::out_of_range& e) {
        return new StringDecodeResult(out_range_text_length);
    }
}

