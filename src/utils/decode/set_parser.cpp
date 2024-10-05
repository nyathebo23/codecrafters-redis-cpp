#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>
#include <cmath>
#include <map>
#include <unordered_set>
#include "simple_data_parser.h"
#include "map_parser.h"
#include "array_parser.h"
#include "set_parser.h"

using SetAndInd = std::pair<std::unordered_set<std::any>, int>;
using SetResp = std::pair<std::variant<SetAndInd, std::string>, bool>;
SetResp parse_decode_unordered_set(const std::string& msg){
    auto pair_token = parse_decode_array(msg);
    if (!pair_token.second)
        return std::make_pair("Array format error", false);

    auto pair_array_resp = std::get<ArrayAndInd>(pair_token.first);
    auto array = pair_array_resp.first;
    int end_symbol_pos = pair_array_resp.second;

    std::unordered_set<std::any> set_datas(std::begin(array), std::end(array));
    if (set_datas.size() != array.size())
        return std::make_pair("Array format error", false);    

    return std::make_pair(std::make_pair(set_datas, end_symbol_pos), true);      

}