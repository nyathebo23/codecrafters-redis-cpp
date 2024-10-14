
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <variant>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "slave_socket_management.h"
#include "decode/array_parser_dec.h"
#include "decode/small_aggregate_parser_dec.h"
#include "process_rdbfile_datas.h"
#include "decode/simple_data_parser_dec.h"
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "command_processing.h"
#include "socket_management.h"
#include "global_datas.h"



SlaveSocketManagement::SlaveSocketManagement(short family, int type, std::map<std::string, std::string> extra) 
: SocketManagement(family, type, extra){
} ;


void SlaveSocketManagement::execute_command(std::string buffer_data, const int& clientfd) {
    auto command_elts = this->get_command_array_from_rawdata(buffer_data);
    std::string cmd = command_elts.first;
    std::vector<std::string> extra_params = command_elts.second;

    if (cmd == "echo"){
        CommandProcessing::echo(extra_params, clientfd);
    }
    else if (cmd == "ping"){
        CommandProcessing::ping(clientfd);
    }
    else if (cmd == "set"){
        CommandProcessing::set_without_send(extra_params);
    }
    else if (cmd == "get"){
        CommandProcessing::get(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "config"){
        CommandProcessing::config(extra_params, clientfd, extra_args);
    }
    else if (cmd == "keys"){
        CommandProcessing::keys(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "info"){
        CommandProcessing::info(extra_params, clientfd, "slave");
    }
    else if (cmd == "replconf"){
        CommandProcessing::replconf(extra_params, clientfd);
    }
}

