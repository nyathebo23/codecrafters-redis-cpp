#include "resp_constants.h"
#include "encode/simple_data_parser_enc.h"

const std::string okResp = parse_encode_simple_string("OK");
const std::string pongResp = parse_encode_simple_string("PONG");
Encoder* replconfEnc = &BulkStringEncoder("REPLCONF");
Encoder* ackEnc = &BulkStringEncoder("ACK");
Encoder* getackEnc = &BulkStringEncoder("GETACK");
Encoder* capaEnc = &BulkStringEncoder("capa");
Encoder* portListeningEnc = &BulkStringEncoder("listening-port");
Encoder* psync2Enc = &BulkStringEncoder("psync2");
Encoder* psyncEnc = &BulkStringEncoder("PSYNC");
Encoder* questionMarkEnc = &BulkStringEncoder("?");
Encoder* minusOneEnc = &BulkStringEncoder("-1");
Encoder* starEnc = &BulkStringEncoder("*");

const std::string stringTypeResp = parse_encode_simple_string("string");
const std::string streamTypeResp = parse_encode_simple_string("stream");
const std::string listTypeResp = parse_encode_simple_string("list");
const std::string noneTypeResp = parse_encode_simple_string("none");