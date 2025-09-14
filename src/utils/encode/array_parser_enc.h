#ifndef ARRAY_PARSER_ENC_H 
#define ARRAY_PARSER_ENC_H

#include <utility>
#include <string>
#include <vector>
#include <map>
#include "../../globals_datas/stream.h"
#include "encoders.h"

std::string parse_encode_array(const std::vector<EncoderPtr>& msg);
std::string parse_encode_string_array(const std::vector<std::string>& msg);
std::string parse_encode_array_for_xrange(const VectorMapEntries data);
std::string parse_encode_array_for_xread(const std::vector<std::pair<std::string, VectorMapEntries>> data);

#endif