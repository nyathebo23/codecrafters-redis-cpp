#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <unistd.h>

#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "encode/encoders.h"
#include "decode/simple_data_parser_dec.h"
#include "command_processing.h"
#include "list_commands_processing.h"
#include "resp_constants.h"
#include "../globals_datas/global_datas.h"

std::string ListCommandsProcessing::rpush(std::vector<std::string> extras) {
    std::vector<std::string> values;
    std::copy(extras.begin()+1, extras.end(), std::back_inserter(values));
    long size = GlobalDatas::lists.append_right(extras[0], values);
    return parse_encode_integer(size);
}

std::string ListCommandsProcessing::lrange(const std::vector<DecodedResultPtr>& extras) {
    std::string list_key = extras[0]->asString();
    std::transform(list_key.begin(), list_key.end(), list_key.begin(), ::tolower);
    std::vector<std::string> result = GlobalDatas::lists.left_range(list_key, extras[1]->asInteger(), extras[2]->asInteger());
    return parse_encode_string_array(result);
}

std::string ListCommandsProcessing::lpush(std::vector<std::string> extras) {
    std::vector<std::string> values;
    std::copy(extras.begin()+1, extras.end(), std::back_inserter(values));
    long size = GlobalDatas::lists.append_left(extras[0], values);
    return parse_encode_integer(size);
}

std::string ListCommandsProcessing::llen(std::vector<std::string> extras) {
    long size = GlobalDatas::lists.get_size(extras[0]);
    return parse_encode_integer(size);
}

std::string ListCommandsProcessing::lpop(const std::vector<DecodedResultPtr>& extras) {
    if (extras.size() == 1) {
        auto value = GlobalDatas::lists.left_pop(extras[0]->asString());
        if (value.has_value()) {
            return parse_encode_bulk_string(value.value());
        } 
        else {
            return NULL_BULK_STRING;
        }
    }
    else if (extras.size() == 2) {
        auto values = GlobalDatas::lists.left_pop_list(extras[0]->asString(), extras[1]->asInteger());
        return parse_encode_string_array(values);
    }
    return "";
}

std::string ListCommandsProcessing::blpop(std::vector<std::string> extras) {
    return "";
}