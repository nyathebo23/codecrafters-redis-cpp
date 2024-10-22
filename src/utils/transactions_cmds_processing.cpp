#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <any>
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/simple_data_parser_dec.h"
#include "command_processing.h"
#include "transactions_cmds_processing.h"
#include "global_datas.h"



void TransactionsCmdsProcessing::multi(int dest_fd){
    CommandProcessing::send_data(parse_encode_simple_string("OK"), dest_fd);
};

void TransactionsCmdsProcessing::exec(bool& is_queue_active, std::vector<std::string>& cmds_to_exec, int dest_fd){
    if (!is_queue_active){
        std::string err_msg = "ERR EXEC without MULTI";
        CommandProcessing::send_data(parse_encode_error_msg(err_msg), dest_fd);
    }
    else {
        if (cmds_to_exec.size() == 0){
            is_queue_active = false;
            CommandProcessing::send_data("*0\r\n", dest_fd);
        }
        else {
            is_queue_active = false;
            cmds_to_exec.clear();            
        }
    }
};

void TransactionsCmdsProcessing::discard(bool& is_queue_active, std::vector<std::string>& cmds_to_exec, int dest_fd){
    if (!is_queue_active){
        std::string err_msg = "ERR DISCARD without MULTI";
        CommandProcessing::send_data(parse_encode_error_msg(err_msg), dest_fd);
    }
    else {
        is_queue_active = false;
        cmds_to_exec.clear();
    }
};