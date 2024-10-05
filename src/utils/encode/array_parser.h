#ifndef ARRAY_PARSER_H // Prévention des inclusions multiples
#define ARRAY_PARSER_H

#include <utility>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <any>

// Déclaration des fonctions
std::string parse_encode_array(const std::vector<std::any>&);

#endif