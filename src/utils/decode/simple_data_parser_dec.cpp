#include <iostream>
#include <string>
#include <cmath>
#include "simple_data_parser_dec.h"

bool detect_wrong_char_in_substring(const std::string& str){
    if (str.find("\r\n") != std::string::npos)
        return true;
    return false;
};
std::string encod_error = "Error in encoding";

StringDecodeResult* parse_decode_simple_string(const std::string& msg){
    const std::string STR = msg.substr(1, msg.length()-3);
    if (detect_wrong_char_in_substring(STR))
        return new StringDecodeResult(encod_error);
    return new StringDecodeResult(STR, std::nullopt);
}

StringDecodeResult* parse_decode_error_msg(const std::string& msg){
    const std::string STR = msg.substr(1, msg.length()-3);
    if (detect_wrong_char_in_substring(STR))
        return new StringDecodeResult(encod_error);
    return new StringDecodeResult(STR, std::nullopt);
}

IntDecodeResult* parse_decode_integer(const std::string& msg){
    const std::string STRNUM = msg.substr(1, msg.length()-3);
    try {
        long number = std::stol(STRNUM);
        return new IntDecodeResult(number);
    } catch (const std::invalid_argument& e) {
        return new IntDecodeResult(STRNUM + "is not a number");
    } catch (const std::out_of_range& e) {
        return new IntDecodeResult(STRNUM + "is too big to be converted into number");
    }
}

BooleanDecodeResult* parse_decode_boolean(const std::string& msg){
    if (msg.compare("#t\r\n") == 0)
        return new BooleanDecodeResult(true);
    else if (msg.compare("#f\r\n") == 0)
        return new BooleanDecodeResult(false);
    else return new BooleanDecodeResult(encod_error);
}

DoubleDecodeResult* parse_decode_double(const std::string& msg){
    std::size_t pos_e = msg.find('e');
    if (pos_e == std::string::npos)
        pos_e = msg.find('E');
    try {
        if (pos_e == std::string::npos){
            const std::string STRNUM = msg.substr(1, msg.length() - 3);
            double number = std::stod(STRNUM);
            return new DoubleDecodeResult(number);
        }
        else {
            const std::string EXP = msg.substr(pos_e + 1, msg.length() - 3 - pos_e);
            const std::string STRNUM = msg.substr(1, pos_e - 1);
            int expint = std::stoi(EXP);
            double number = std::stod(STRNUM) * std::pow(10, expint);
            return new DoubleDecodeResult(number);
        }
    } catch (const std::invalid_argument& e) {
        return new DoubleDecodeResult("Not a number");
    } catch (const std::out_of_range& e) {
        return new DoubleDecodeResult("Too big number");
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
StringDecodeResult* parse_decode_big_number(const std::string& msg){
    const std::string NUM = msg.substr(1, msg.length() - 3);
    if (detect_wrong_char_in_substring(NUM) || !is_digits(NUM))
        return new StringDecodeResult("", "Error");
    return new StringDecodeResult(NUM, std::nullopt); 
}

