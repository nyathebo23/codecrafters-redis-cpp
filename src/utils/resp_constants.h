#ifndef RESP_CONSTANTS_H
#define RESP_CONSTANTS_H

#include "encode/encoders.h"

extern const std::string okResp;
extern const std::string pongResp;
extern const BulkStringEncoder replconfEnc;
extern const BulkStringEncoder ackEnc;
extern const BulkStringEncoder getackEnc;
extern const BulkStringEncoder capaEnc;
extern const BulkStringEncoder portListeningEnc;
extern const BulkStringEncoder psync2Enc;
extern const BulkStringEncoder psyncEnc;
extern const BulkStringEncoder questionMarkEnc;
extern const BulkStringEncoder minusOneEnc;
extern const BulkStringEncoder starEnc;
extern const std::string stringTypeResp;
extern const std::string streamTypeResp;
extern const std::string listTypeResp;
extern const std::string noneTypeResp;

#endif