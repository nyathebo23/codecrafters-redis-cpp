#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/simple_data_parser_dec.h"
#include "command_processing.h"
#include "transactions_cmds_processing.h"
#include "global_datas.h"



void TransactionsCmdsProcessing::multi(int dest_fd){
    GlobalDatas::is_queue_active = true;
    CommandProcessing::send_data(parse_encode_simple_string("OK"), dest_fd);
};

void TransactionsCmdsProcessing::exec(int dest_fd){
    if (!GlobalDatas::is_queue_active){
        std::string err_msg = "ERR EXEC without MULTI";
        CommandProcessing::send_data(parse_encode_error_msg(err_msg), dest_fd);
    }
    else {
        if (GlobalDatas::cmds_to_exec.size() == 0){
            CommandProcessing::send_data("*0\r\n", dest_fd);
        }
        else {

        }
    }
};

void TransactionsCmdsProcessing::discard(int dest_fd){
    if (!GlobalDatas::is_queue_active){
        std::string err_msg = "ERR DISCARD without MULTI";
        CommandProcessing::send_data(parse_encode_error_msg(err_msg), dest_fd);
    }
    else {
        GlobalDatas::is_queue_active = false;
        GlobalDatas::cmds_to_exec.clear();
    }
};