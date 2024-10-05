#ifndef MAP_PARSER_H // Prévention des inclusions multiples
#define MAP_PARSER_H

#include <utility>
#include <string>
#include <variant>
#include <map>
#include <any>

// Déclaration des fonctions
using MapAndInd = std::pair<std::map<std::any, std::any>, int>;
using MapResp = std::pair<std::variant<MapAndInd, std::string>, bool>;
MapResp parse_decode_map(const std::string&);

#endif
