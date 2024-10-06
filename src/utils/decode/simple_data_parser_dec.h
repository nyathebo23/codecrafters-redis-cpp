#ifndef SIMPLE_DATA_PARSER_DEC_H // Prévention des inclusions multiples
#define SIMPLE_DATA_PARSER_DEC_H

#include <utility>
#include <string>
#include <variant>

// Déclaration des fonctions
extern std::pair<std::string, bool> parse_decode_simple_string(const std::string&);
extern std::pair<std::string, bool> parse_decode_error_msg(const std::string&);
extern std::pair<std::variant<int, std::string>, bool> parse_decode_integer(const std::string&);
extern std::pair<std::variant<bool, std::string>, bool> parse_decode_boolean(const std::string&);
extern std::pair<std::variant<double, std::string>, bool> parse_decode_double(const std::string&);
extern std::pair<std::string, bool> parse_decode_big_number(const std::string&);
bool detect_wrong_char_in_substring(const std::string&);

#endif


