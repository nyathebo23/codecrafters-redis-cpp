#ifndef SIMPLE_DATA_PARSER_ENC_H 
#define SIMPLE_DATA_PARSER_ENC_H

#include <utility>
#include <string>
#include <variant>

std::string parse_encode_simple_string(const std::string&);
std::string parse_encode_error_msg(const std::string&);
std::string parse_encode_integer(const long&);
std::string parse_encode_boolean(const bool&);
std::string parse_encode_double(const double&);
std::string parse_encode_big_number(const std::string&);

#endif


