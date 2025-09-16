#include "sortedsets_commands_processing.h"
#include "command_processing.h"
#include "../globals_datas/global_datas.h"
#include "../utils/encode/simple_data_parser_enc.h"
#include "../utils/encode/small_aggregate_parser_enc.h"
#include "../utils/encode/array_parser_enc.h"
#include "../utils/resp_constants.h"

std::string SortedSetsCommandsProcessing::zadd(std::vector<std::string> extras) {
    if (extras.size() != 3) {
        return CommandProcessing::params_count_error("zadd");
    }
    double score = std::stod(extras[1]);
    long res = GlobalDatas::sortedSets.zadd(extras[0], score, extras[2]);
    return parse_encode_integer(res);
};

std::string SortedSetsCommandsProcessing::zrank(std::vector<std::string> extras) {
    if (extras.size() != 2) {
        return CommandProcessing::params_count_error("zrank");
    }
    auto res = GlobalDatas::sortedSets.zrank(extras[0], extras[1]);
    if (res.has_value())
        return parse_encode_integer(res.value());
    return NULL_BULK_STRING;
};

std::string SortedSetsCommandsProcessing::zrange(std::vector<std::string> extras) {
    if (extras.size() != 3) {
        return CommandProcessing::params_count_error("zrange");       
    }
    long start = std::stol(extras[1]);
    long end = std::stol(extras[2]);
    std::vector<std::string> result = GlobalDatas::sortedSets.zrange(extras[0], start, end);
    return parse_encode_string_array(result);
};

std::string SortedSetsCommandsProcessing::zcard(std::vector<std::string> extras) {
    if (extras.size() != 1) {
        return CommandProcessing::params_count_error("zcard");
    }
    return parse_encode_integer(GlobalDatas::sortedSets.zcard(extras[0]));
};

std::string SortedSetsCommandsProcessing::zscore(std::vector<std::string> extras) {
    if (extras.size() != 2) {
        return CommandProcessing::params_count_error("zscore");
    }
    std::string res = GlobalDatas::sortedSets.zscore(extras[0], extras[1]);
    if (res == "") 
        return NULL_BULK_STRING;
    return parse_encode_bulk_string(res);
};

std::string SortedSetsCommandsProcessing::zrem(std::vector<std::string> extras) {
    if (extras.size() != 2) {
        return CommandProcessing::params_count_error("zrem");
    }
    return parse_encode_integer(GlobalDatas::sortedSets.zrem(extras[0], extras[1]));
};