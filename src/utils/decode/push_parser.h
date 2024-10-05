#ifndef PUSH_PARSER_H // Prévention des inclusions multiples
#define PUSH_PARSER_H

#include <utility>
#include <string>
#include <variant>
#include <vector>
#include <any>

using ArrayPushResp = std::pair<std::variant<std::vector<std::any>, std::string>, bool>;
extern ArrayPushResp parse_decode_push(const std::string&);

#endif