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


static void TransactionsCmdsProcessing::multi(int dest_fd){
    
};

static void TransactionsCmdsProcessing::exec(int dest_fd){

};

static void TransactionsCmdsProcessing::discard(int dest_fd){

};