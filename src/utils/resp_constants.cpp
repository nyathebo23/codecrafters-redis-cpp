#include "resp_constants.h"
#include "encode/simple_data_parser_enc.h"

const std::string okResp = parse_encode_simple_string("OK");
const std::string pongResp = parse_encode_simple_string("PONG");
BulkStringEncoder replconfEnc = BulkStringEncoder("REPLCONF");
BulkStringEncoder ackEnc = BulkStringEncoder("ACK");
BulkStringEncoder getackEnc = BulkStringEncoder("GETACK");
BulkStringEncoder capaEnc = BulkStringEncoder("capa");
BulkStringEncoder portListeningEnc = BulkStringEncoder("listening-port");
BulkStringEncoder psync2Enc = BulkStringEncoder("psync2");
BulkStringEncoder psyncEnc = BulkStringEncoder("PSYNC");
BulkStringEncoder questionMarkEnc = BulkStringEncoder("?");
BulkStringEncoder minusOneEnc = BulkStringEncoder("-1");
BulkStringEncoder starEnc = BulkStringEncoder("*");
BulkStringEncoder pingEnc = BulkStringEncoder("PING");
const std::string stringTypeResp = parse_encode_simple_string("string");
const std::string streamTypeResp = parse_encode_simple_string("stream");
const std::string listTypeResp = parse_encode_simple_string("list");
const std::string noneTypeResp = parse_encode_simple_string("none");

