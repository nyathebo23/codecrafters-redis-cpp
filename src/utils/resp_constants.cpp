#include "resp_constants.h"
#include "encode/simple_data_parser_enc.h"

const std::string NULL_BULK_STRING = "$-1\r\n";
const std::string okResp = parse_encode_simple_string("OK");
const std::string pongResp = parse_encode_simple_string("PONG");
const std::string queueResp = parse_encode_simple_string("QUEUED");
const std::string replconfGetackCmd = "*3\r\n$8\r\nREPLCONF\r\n$6\r\nGETACK\r\n$1\r\n*\r\n";
const std::string replconfMsg1PartEnc = "*3\r\n$8\r\nREPLCONF\r\n$14\r\nlistening-port\r\n";
const std::string replconfAckPartEnc = "*3\r\n$8\r\nREPLCONF\r\n$3\r\nACK\r\n$";
const std::string replconfMsg2Enc = "*3\r\n$8\r\nREPLCONF\r\n$4\r\ncapa\r\n$6\r\npsync2\r\n";
const std::string psyncMsgEnc = "*3\r\n$5\r\nPSYNC\r\n$1\r\n?\r\n$2\r\n-1\r\n";
const std::string pingAsListEnc = "*1\r\n$4\r\nPING\r\n";
const std::string stringTypeResp = parse_encode_simple_string("string");
const std::string streamTypeResp = parse_encode_simple_string("stream");
const std::string listTypeResp = parse_encode_simple_string("list");
const std::string noneTypeResp = parse_encode_simple_string("none");

