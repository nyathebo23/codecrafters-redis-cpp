#include "channels_commands_processing.h"
#include "../utils/encode/simple_data_parser_enc.h"
#include "../utils/encode/small_aggregate_parser_enc.h"
#include "../utils/resp_constants.h"
#include "../globals_datas/global_datas.h"
#include "command_processing.h"
#include <iostream>

void ChannelsCommandsProcessing::subsribe(std::vector<std::string> extras, int clientfd) {
    if (extras.size() != 1) {
        std::string errorMsg = CommandProcessing::params_count_error("subscribe");
        CommandProcessing::send_data(parse_encode_error_msg(errorMsg), clientfd);
    }
    std::string channelname = extras[0];
    long count = GlobalDatas::channels.subscribe(channelname, clientfd);
    std::string channelNameEnc = parse_encode_bulk_string(channelname);
    std::string resp = "*3\r\n" + SUBSCRIBE_ENC + channelNameEnc + parse_encode_integer(count);
    CommandProcessing::send_data(resp, clientfd);
};

void ChannelsCommandsProcessing::enter_subscribe_mode(int clientfd) {
    while (1) {
        auto commandOpt = CommandProcessing::receive_command_from_client(clientfd);
        if (!commandOpt.has_value())
            break;
        Command command = commandOpt.value();
        if (command.error.has_value()){
            CommandProcessing::send_data(parse_encode_error_msg(command.error.value()), clientfd);
            continue;
        }
        std::string cmd = command.name;
        std::cout << cmd + " command";
        if (cmd == "subscribe") {
            ChannelsCommandsProcessing::subsribe(command.args, clientfd);
        }
        else if (cmd == "unsubscribe") {
            ChannelsCommandsProcessing::unsubscribe(command.args, clientfd);
        }
        else if (cmd == "publish") {
            ChannelsCommandsProcessing::publish(command.args, clientfd);
        }
        else if (cmd == "ping") {
            ChannelsCommandsProcessing::ping(command.args, clientfd);
        }
        if (cmd == "psubscribe") {

        }
        else if (cmd == "punsubscribe") {

        }
        else if (cmd == "quit") {
            break;
        }
        else {
            std::string err = ChannelsCommandsProcessing::unallowed_command_error(cmd);
            CommandProcessing::send_data(parse_encode_error_msg(err), clientfd);
        }
    }

}


void ChannelsCommandsProcessing::unsubscribe(std::vector<std::string> extras, int clientfd) {
    if (extras.size() != 1) {
        std::string errorMsg = CommandProcessing::params_count_error("unsubscribe");
        CommandProcessing::send_data(parse_encode_error_msg(errorMsg), clientfd);
    }
    std::string channelname = extras[0];
    long count = GlobalDatas::channels.unsubscribe(channelname, clientfd);
    std::string channelNameEnc = parse_encode_bulk_string(channelname);
    std::string resp = "*3\r\n" + SUBSCRIBE_ENC + channelNameEnc + parse_encode_integer(count);
    CommandProcessing::send_data(resp, clientfd);
};

void ChannelsCommandsProcessing::publish(std::vector<std::string> extras, int clientfd) {
    if (extras.size() != 2) {
        std::string errorMsg = CommandProcessing::params_count_error("publish");
        CommandProcessing::send_data(parse_encode_error_msg(errorMsg), clientfd);
    }
    std::string channelname = extras[0];
    std::vector<int> clientfdList = GlobalDatas::channels.get_clients_fd(channelname);
    std::string msg = extras[1];
    std::string channelNameEnc = parse_encode_bulk_string(channelname);
    CommandProcessing::send_data(parse_encode_integer(clientfdList.size()), clientfd);

    for (int fd: clientfdList) {
        std::string resp = "*3\r\n$7\r\nmessage\r\n" + channelNameEnc + parse_encode_bulk_string(msg);
        CommandProcessing::send_data(resp, fd);
    }
};


void ChannelsCommandsProcessing::ping(std::vector<std::string> extras, int clientfd) {
    if (extras.size() != 0) {
        std::string errorMsg = CommandProcessing::params_count_error("subscribe mode ping");
        CommandProcessing::send_data(parse_encode_error_msg(errorMsg), clientfd);
    }
    CommandProcessing::send_data(PING_SUBS_MODE_RESP, clientfd);
};