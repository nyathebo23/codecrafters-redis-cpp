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
#include <thread>




void ListCommandsProcessing::rpush(std::vector<std::string> extras, int clientfd) {
    if (extras.size() < 2) {
        std::string errResp = parse_encode_error_msg("rpush command format error");
        CommandProcessing::send_data(errResp, clientfd);
        return;
    }
    std::string listkey = extras[0];
    std::vector<std::string> values;
    std::copy(extras.begin()+1, extras.end(), std::back_inserter(values));
    long size = GlobalDatas::lists.append_right(listkey, values);
    CommandProcessing::send_data(parse_encode_integer(size), clientfd);
    try_run_waiting_lpop(listkey);
}

std::string ListCommandsProcessing::lrange(std::vector<std::string> extras) {
    if (extras.size() != 3) {
        return parse_encode_error_msg("lrange command format error");
    }
    std::string list_key = extras[0];
    std::transform(list_key.begin(), list_key.end(), list_key.begin(), ::tolower);
    long start = std::stol(extras[1]);
    long end = std::stol(extras[2]);
    std::vector<std::string> result = GlobalDatas::lists.left_range(list_key, start, end);
    return parse_encode_string_array(result);
}

void ListCommandsProcessing::lpush(std::vector<std::string> extras, int clientfd) {
    if (extras.size() < 2) {
        std::string errResp = parse_encode_error_msg("lpush command format error");
        CommandProcessing::send_data(errResp, clientfd);
        return;
    }
    std::string listkey = extras[0];
    std::vector<std::string> values;
    std::copy(extras.begin()+1, extras.end(), std::back_inserter(values));
    long size = GlobalDatas::lists.append_left(listkey, values);
    CommandProcessing::send_data(parse_encode_integer(size), clientfd);
    try_run_waiting_lpop(listkey);
}

std::string ListCommandsProcessing::llen(std::vector<std::string> extras) {
    if (extras.size() != 1) {
        return parse_encode_error_msg("lrange command format error");
    }
    long size = GlobalDatas::lists.get_size(extras[0]);
    return parse_encode_integer(size);
}

std::string ListCommandsProcessing::lpop(std::vector<std::string> extras) {
    if (extras.size() == 1) {
        auto value = GlobalDatas::lists.left_pop(extras[0]);
        if (value.has_value()) {
            return parse_encode_bulk_string(value.value());
        } 
        else {
            return NULL_BULK_STRING;
        }
    }
    else if (extras.size() == 2) {
        unsigned long count = std::stoul(extras[1]);
        auto values = GlobalDatas::lists.left_pop_list(extras[0], count);
        return parse_encode_string_array(values);
    }
    return parse_encode_error_msg("lpop command format error");
}

void ListCommandsProcessing::blpop(std::vector<std::string> extras, int clientfd) {
    if (extras.size() != 2) {
        std::string err = parse_encode_error_msg("blpop command format error");
        CommandProcessing::send_data(err, clientfd);
        return;
    }
    unsigned long timeout = std::stoul(extras[1]);
    std::string key = extras[0];
    GlobalDatas::lists.addOnWaitingBLPOPList(key, clientfd);
    if (timeout > 0) {
        check_blpop_exec(timeout, key, clientfd);
    }
}

void ListCommandsProcessing::try_run_waiting_lpop(std::string list_key) {
    if (GlobalDatas::lists.isClientWaitingBLPOP(list_key)) {
        auto value = GlobalDatas::lists.left_pop(list_key).value();
        std::vector<std::string> resp_array = {list_key, value};
        int clientfd = GlobalDatas::lists.getFirstClient(list_key);
        CommandProcessing::send_data(parse_encode_string_array(resp_array), clientfd);
    }
}

void ListCommandsProcessing::check_blpop_exec(unsigned long delay, const std::string& list_key, int clientfd) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    if (GlobalDatas::lists.checkAndDeleteClientWaiting(list_key, clientfd)) {
        CommandProcessing::send_data(NULL_BULK_STRING, clientfd);
    }
}