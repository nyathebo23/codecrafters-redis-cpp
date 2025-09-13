#include "encode/simple_data_parser_enc.h"
#include "encode/encoders.h"

extern const std::string okResp = parse_encode_simple_string("OK");
extern const std::string pongResp = parse_encode_simple_string("PONG");
extern const Encoder* replconfEnc = &BulkStringEncoder("REPLCONF");
extern const Encoder* ackEnc = &BulkStringEncoder("ACK");
extern const Encoder* getackEnc = &BulkStringEncoder("GETACK");
extern const Encoder* capaEnc = &BulkStringEncoder("capa");
extern const Encoder* portListeningEnc = &BulkStringEncoder("listening-port");
extern const Encoder* psync2Enc = &BulkStringEncoder("psync2");
extern const Encoder* psyncEnc = &BulkStringEncoder("PSYNC");
extern const Encoder* questionMarkEnc = &BulkStringEncoder("?");
extern const Encoder* minusOneEnc = &BulkStringEncoder("-1");
extern const Encoder* starEnc = &BulkStringEncoder("*");
extern const std::string stringTypeResp = parse_encode_simple_string("string");
extern const std::string streamTypeResp = parse_encode_simple_string("stream");
extern const std::string listTypeResp = parse_encode_simple_string("list");
extern const std::string noneTypeResp = parse_encode_simple_string("none");
