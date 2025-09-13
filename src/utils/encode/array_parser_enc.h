#ifndef ARRAY_PARSER_ENC_H // Prévention des inclusions multiples
#define ARRAY_PARSER_ENC_H

#include <utility>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <any>
#include "../../globals_datas/stream.h"
#include "encoders.h"

// Déclaration des fonctions
std::string parse_encode_array(const std::vector<Encoder*>& msg);
std::string parse_encode_array_for_xrange(const VectorMapEntries data);
std::string parse_encode_array_for_xread(const std::vector<std::pair<std::string, VectorMapEntries>> data);

#endif