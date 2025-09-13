#include "encode/simple_data_parser_enc.h"
#include "encode/encoders.h"

extern const std::string okResp = parse_encode_simple_string("OK");
extern const std::string pongResp = parse_encode_simple_string("PONG");
extern const BulkStringEncoder replconfEnc = BulkStringEncoder("REPLCONF");
extern const BulkStringEncoder ackEnc = BulkStringEncoder("ACK");
extern const BulkStringEncoder getackEnc = BulkStringEncoder("GETACK");
extern const BulkStringEncoder capaEnc = BulkStringEncoder("capa");
extern const BulkStringEncoder portListeningEnc = BulkStringEncoder("listening-port");
extern const BulkStringEncoder psync2Enc = BulkStringEncoder("psync2");
extern const BulkStringEncoder psyncEnc = BulkStringEncoder("PSYNC");
extern const BulkStringEncoder questionMarkEnc = BulkStringEncoder("?");
extern const BulkStringEncoder minusOneEnc = BulkStringEncoder("-1");
extern const BulkStringEncoder starEnc = BulkStringEncoder("*");
extern const BulkStringEncoder stringTypeResp = parse_encode_simple_string("string");
extern const BulkStringEncoder streamTypeResp = parse_encode_simple_string("stream");
extern const BulkStringEncoder listTypeResp = parse_encode_simple_string("list");
extern const BulkStringEncoder noneTypeResp = parse_encode_simple_string("none");
