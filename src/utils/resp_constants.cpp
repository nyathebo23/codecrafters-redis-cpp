#include "resp_constants.h"
#include "encode/simple_data_parser_enc.h"

const std::string okResp = parse_encode_simple_string("OK");
const std::string pongResp = parse_encode_simple_string("PONG");
const BulkStringEncoder replconfEnc = BulkStringEncoder("REPLCONF");
const BulkStringEncoder ackEnc = BulkStringEncoder("ACK");
const BulkStringEncoder getackEnc = BulkStringEncoder("GETACK");
const BulkStringEncoder capaEnc = BulkStringEncoder("capa");
const BulkStringEncoder portListeningEnc = BulkStringEncoder("listening-port");
const BulkStringEncoder psync2Enc = BulkStringEncoder("psync2");
const BulkStringEncoder psyncEnc = BulkStringEncoder("PSYNC");
const BulkStringEncoder questionMarkEnc = BulkStringEncoder("?");
const BulkStringEncoder minusOneEnc = BulkStringEncoder("-1");
const BulkStringEncoder starEnc = BulkStringEncoder("*");
const std::string stringTypeResp = parse_encode_simple_string("string");
const std::string streamTypeResp = parse_encode_simple_string("stream");
const std::string listTypeResp = parse_encode_simple_string("list");
const std::string noneTypeResp = parse_encode_simple_string("none");