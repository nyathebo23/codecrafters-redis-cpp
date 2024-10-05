#ifndef MAP_PARSER_H // Prévention des inclusions multiples
#define MAP_PARSER_H

#include <utility>
#include <string>
#include <variant>
#include <map>
#include <any>

// Déclaration des fonctions

std::string parse_encode_map(const std::map<std::any, std::any>&);

#endif
