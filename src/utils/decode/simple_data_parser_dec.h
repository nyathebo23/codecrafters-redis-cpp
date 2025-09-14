#ifndef SIMPLE_DATA_PARSER_DEC_H // Prévention des inclusions multiples
#define SIMPLE_DATA_PARSER_DEC_H

#include <utility>
#include <string>
#include "decoders.h"

// Déclaration des fonctions
StringDecodeResult* parse_decode_simple_string(const std::string&);
StringDecodeResult* parse_decode_error_msg(const std::string&);
IntDecodeResult* parse_decode_integer(const std::string&);
BooleanDecodeResult* parse_decode_boolean(const std::string&);
DoubleDecodeResult* parse_decode_double(const std::string&);
StringDecodeResult* parse_decode_big_number(const std::string&);
bool detect_wrong_char_in_substring(const std::string&);

#endif


