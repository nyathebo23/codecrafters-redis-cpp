#ifndef RESP_CONSTANTS_H
#define RESP_CONSTANTS_H

#include "encode/encoders.h"

extern const std::string okResp;
extern const std::string pongResp;
extern Encoder* replconfEnc;
extern Encoder* ackEnc;
extern Encoder* getackEnc;
extern Encoder* capaEnc;
extern Encoder* portListeningEnc;
extern Encoder* psync2Enc;
extern Encoder* psyncEnc;
extern Encoder* questionMarkEnc;
extern Encoder* minusOneEnc;
extern Encoder* starEnc;
extern const std::string stringTypeResp;
extern const std::string streamTypeResp;
extern const std::string listTypeResp;
extern const std::string noneTypeResp;

#endif