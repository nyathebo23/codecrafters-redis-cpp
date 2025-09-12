#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <thread>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/simple_data_parser_dec.h"
#include "command_processing.h"
#include "list_commands_processing.h"
#include "../globals_datas/global_datas.h"

std::string ListCommandsProcessing::rpush(std::vector<std::string> extras) {
    std::vector<std::string> values;
    std::copy(extras.begin()+1, extras.end(), std::back_inserter(values));
    int size = GlobalDatas::lists.append_right(extras[0], values);
    return parse_encode_integer(size);
}

std::string ListCommandsProcessing::lrange(std::vector<std::any> extras) {
    std::string list_key = std::any_cast<std::string>(extras[0]);
    int start = std::any_cast<int>(extras[1]);
    int end = std::any_cast<int>(extras[2]);
    std::vector<std::string> result = GlobalDatas::lists.left_range(list_key, start, end);
    return "";
    //return parse_encode_array(result);
}

std::string ListCommandsProcessing::lpush(std::vector<std::string> extras) {
    std::vector<std::string> values;
    std::copy(extras.begin()+1, extras.end(), std::back_inserter(values));
    int size = GlobalDatas::lists.append_left(extras[0], values);
    return parse_encode_integer(size);
}

std::string ListCommandsProcessing::llen(std::vector<std::string> extras) {
    return "";

}

std::string ListCommandsProcessing::lpop(std::vector<std::any> extras) {
    return "";

}

std::string ListCommandsProcessing::blpop(std::vector<std::any> extras) {
    return "";

}