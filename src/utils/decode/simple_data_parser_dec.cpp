#include <iostream>
#include <string>
#include <cmath>
#include "simple_data_parser_dec.h"

bool detect_wrong_char_in_substring(const std::string& str){
    if (str.find("\r\n") != std::string::npos)
        return true;
    return false;
};
std::string encError = "Error in encoding";

StringDecodeResult parse_decode_simple_string(const std::string& msg){
    const std::string str = msg.substr(1, msg.length()-3);
    if (detect_wrong_char_in_substring(str))
        return StringDecodeResult(encError);
    return StringDecodeResult(str, std::nullopt);
}

StringDecodeResult parse_decode_error_msg(const std::string& msg){
    const std::string str = msg.substr(1, msg.length()-3);
    if (detect_wrong_char_in_substring(str))
        return StringDecodeResult(encError);
    return StringDecodeResult(str, std::nullopt);
}

IntDecodeResult parse_decode_integer(const std::string& msg){
    const std::string strnum = msg.substr(1, msg.length()-3);
    try {
        long number = std::stol(strnum);
        return IntDecodeResult(number);
    } catch (const std::invalid_argument& e) {
        return IntDecodeResult(strnum + "is not a number");
    } catch (const std::out_of_range& e) {
        return IntDecodeResult(strnum + "is too big to be converted into number");
    }
}

BooleanDecodeResult parse_decode_boolean(const std::string& msg){
    if (msg.compare("#t\r\n") == 0)
        return BooleanDecodeResult(true);
    else if (msg.compare("#f\r\n") == 0)
        return BooleanDecodeResult(false);
    else return BooleanDecodeResult(encError);
}

DoubleDecodeResult parse_decode_double(const std::string& msg){
    std::size_t posE = msg.find('e');
    if (posE == std::string::npos)
        posE = msg.find('E');
    try {
        if (posE == std::string::npos){
            const std::string strnum = msg.substr(1, msg.length() - 3);
            double number = std::stod(strnum);
            return DoubleDecodeResult(number);
        }
        else {
            const std::string exp = msg.substr(posE + 1, msg.length() - 3 - posE);
            const std::string strnum = msg.substr(1, posE - 1);
            int expint = std::stoi(exp);
            double number = std::stod(strnum) * std::pow(10, expint);
            return DoubleDecodeResult(number);
        }
    } catch (const std::invalid_argument& e) {
        return DoubleDecodeResult("Not a number");
    } catch (const std::out_of_range& e) {
        return DoubleDecodeResult("Too big number");
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
StringDecodeResult parse_decode_big_number(const std::string& msg){
    const std::string num = msg.substr(1, msg.length() - 3);
    if (detect_wrong_char_in_substring(num) || !is_digits(num))
        return StringDecodeResult("", "Error");
    return StringDecodeResult(num, std::nullopt); 
}

