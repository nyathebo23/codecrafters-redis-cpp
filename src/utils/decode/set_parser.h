#ifndef SET_PARSER_H // Prévention des inclusions multiples
#define SET_PARSER_H

#include <utility>
#include <string>
#include <variant>
#include <unordered_set>
#include <any>

using SetAndInd = std::pair<std::unordered_set<std::any>, int>;
using SetResp = std::pair<std::variant<SetAndInd, std::string>, bool>;
SetResp parse_decode_unordered_set(const std::string&);

#endif