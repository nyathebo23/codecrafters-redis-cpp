#ifndef ARRAY_PARSER_ENC_H // Prévention des inclusions multiples
#define ARRAY_PARSER_ENC_H

#include <utility>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <any>
#include "../global_datas.h"

// Déclaration des fonctions
std::string parse_encode_array(const std::vector<std::any>&);
std::string parse_encode_array_of_array(const VectorMapEntries data);

#endif