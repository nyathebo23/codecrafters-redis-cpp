#include "resp_constants.h"
#include "encode/simple_data_parser_enc.h"

const std::string okResp = parse_encode_simple_string("OK");
const std::string pongResp = parse_encode_simple_string("PONG");
const Encoder* replconfEnc = &BulkStringEncoder("REPLCONF");
const Encoder* ackEnc = &BulkStringEncoder("ACK");
const Encoder* getackEnc = &BulkStringEncoder("GETACK");
const Encoder* capaEnc = &BulkStringEncoder("capa");
const Encoder* portListeningEnc = &BulkStringEncoder("listening-port");
const Encoder* psync2Enc = &BulkStringEncoder("psync2");
const Encoder* psyncEnc = &BulkStringEncoder("PSYNC");
const Encoder* questionMarkEnc = &BulkStringEncoder("?");
const Encoder* minusOneEnc = &BulkStringEncoder("-1");
const Encoder* starEnc = &BulkStringEncoder("*");

const std::string stringTypeResp = parse_encode_simple_string("string");
const std::string streamTypeResp = parse_encode_simple_string("stream");
const std::string listTypeResp = parse_encode_simple_string("list");
const std::string noneTypeResp = parse_encode_simple_string("none");