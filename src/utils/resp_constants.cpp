#include "resp_constants.h"
#include "encode/simple_data_parser_enc.h"
 
const std::string NULL_BULK_STRING = "$-1\r\n";
const std::string NULL_ARRAY = "*-1\r\n";
const std::string SUBSCRIBE_ENC = "$9\r\nsubscribe\r\n";
const std::string UNSUBSCRIBE_ENC = "$11\r\nunsubscribe\r\n";
const std::string PING_SUBS_MODE_RESP = "*2\r\n$4\r\npong\r\n$0\r\n\r\n";
const std::string OK_RESP = parse_encode_simple_string("OK");
const std::string PONG_RESP = parse_encode_simple_string("PONG");
const std::string QUEUE_RESP = parse_encode_simple_string("QUEUED");
const std::string REPLCONF_GETACK_CMD = "*3\r\n$8\r\nREPLCONF\r\n$6\r\nGETACK\r\n$1\r\n*\r\n";
const std::string REPLCONF_MSG1_PART_ENC = "*3\r\n$8\r\nREPLCONF\r\n$14\r\nlistening-port\r\n";
const std::string REPLCONF_ACK_PART_ENC = "*3\r\n$8\r\nREPLCONF\r\n$3\r\nACK\r\n";
const std::string REPLCONF_MSG2_ENC = "*3\r\n$8\r\nREPLCONF\r\n$4\r\ncapa\r\n$6\r\npsync2\r\n";
const std::string PSYNC_MSG_ENC = "*3\r\n$5\r\nPSYNC\r\n$1\r\n?\r\n$2\r\n-1\r\n";
const std::string PING_AS_LIST_ENC = "*1\r\n$4\r\nPING\r\n";
const std::string STRING_TYPE_RESP = parse_encode_simple_string("string");
const std::string STREAM_TYPE_RESP = parse_encode_simple_string("stream");
const std::string LIST_TYPE_RESP = parse_encode_simple_string("list");
const std::string NONE_TYPE_RESP = parse_encode_simple_string("none");

