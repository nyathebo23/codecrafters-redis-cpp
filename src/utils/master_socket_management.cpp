#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <any>
#include <map>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "master_socket_management.h"
#include "socket_management.h"
#include "command_processing.h"
#include "decode/small_aggregate_parser_dec.h"
#include "process_rdbfile_datas.h"
#include "encode/array_parser_enc.h"
#include "encode/simple_data_parser_enc.h"
#include "encode/small_aggregate_parser_enc.h"
#include "decode/simple_data_parser_dec.h"


int MasterSocketManagement::send_receive_msg_by_command(std::string tosend, std::string toreceive){
    if (send(server_fd, tosend.c_str(), tosend.length(), 0) <= 0){
        std::cout << "Send "+ tosend + " handshake failed";
        //close(server_fd);
        return -1;
    }
    char buffer[128];    
    if (recv(server_fd, &buffer, sizeof(buffer), 0) <= 0) {
        //close(server_fd);
        return -1;
    }
    std::string data(buffer);
    std::string data_decoded = parse_decode_simple_string(data).first;
    // if (data_decoded != toreceive){
    //     std::cout << "Bad message receive to " + tosend + " which is " + data_decoded;
    //     return -1;
    // }
    return 1;
};

MasterSocketManagement::MasterSocketManagement(short family, int type, std::map<std::string, std::string> extra)
: SocketManagement(family, type, extra){

};



void MasterSocketManagement::execute_command(std::string buffer_data, const int& clientfd) {
    auto command_elts = this->get_command_array_from_rawdata(buffer_data);
    std::string cmd = command_elts.first;
    std::vector<std::string> extra_params = command_elts.second;
    if (cmd == "echo"){
        command_processing.echo(extra_params, clientfd);
    }
    else if (cmd == "ping"){
        command_processing.ping(clientfd);
    }
    else if (cmd == "set"){
        for (int replica_fd: this->replicas_fd) {
            if (send(replica_fd, buffer_data.c_str(), buffer_data.length(), 0) <= 0)
                std::cout <<  "replica send msg failed";                
        }
        command_processing.set(extra_params, clientfd);
    }
    else if (cmd == "get"){
        command_processing.get(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "config"){
        command_processing.config(extra_params, clientfd, extra_args);
    }
    else if (cmd == "keys"){
        command_processing.keys(extra_params, clientfd, extra_args["dir"] + "/" + extra_args["dbfilename"]);
    }
    else if (cmd == "info"){
        command_processing.info(extra_params, clientfd, "master");
    }
    else if (cmd == "replconf"){
        command_processing.replconf(extra_params, clientfd);
    }
    else if (cmd == "psync"){
        command_processing.psync(extra_params, clientfd, replicas_fd);
    }
}

int MasterSocketManagement::send_handshake_to_master(int port){
    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cout << "Connect to master failed";
        return -1;
    }
    std::vector<std::any> ping = {std::string("PING")};
    if(send_receive_msg_by_command(parse_encode_array(ping), "PONG") < 0)
        return -1;

    std::vector<std::any> replconf_msg1 = {std::string("REPLCONF"), std::string("listening-port"), std::to_string(port)}; 
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg1), "OK") < 0)
        return -1;
    
    std::vector<std::any> replconf_msg2 = {std::string("REPLCONF"), std::string("capa"), std::string("psync2")};
    if(send_receive_msg_by_command(parse_encode_array(replconf_msg2), "OK") < 0)
        return -1;

    std::vector<std::any> psync_msg = {std::string("PSYNC"), std::string("?"), std::string("-1")};
    if(send_receive_msg_by_command(parse_encode_array(psync_msg), "FULLRESYNC <REPL_ID> 0") < 0)
        return -1;
    char buffer[128];
    if (recv(server_fd, &buffer, sizeof(buffer), 0) <= 0) {
            std::cout << "Don't receive file";
    }    
    
    return 1;
}