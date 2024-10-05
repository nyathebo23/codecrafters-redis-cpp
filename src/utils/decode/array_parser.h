#ifndef ARRAY_PARSER_H // Prévention des inclusions multiples
#define ARRAY_PARSER_H

#include <utility>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <any>

// Déclaration des fonctions
using ArrayAndInd = std::pair<std::vector<std::any>, int>;
using ArrayResp = std::pair<std::variant<ArrayAndInd, std::string>, bool>;
extern ArrayResp parse_decode_array(const std::string&);

#endif