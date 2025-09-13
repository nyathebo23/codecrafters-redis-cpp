#ifndef RESP_CONSTANTS_H
#define RESP_CONSTANTS_H

#include "encode/encoders.h"

extern const std::string okResp;
extern const std::string pongResp;
extern const Encoder* replconfEnc;
extern const Encoder* ackEnc;
extern const Encoder* getackEnc;
extern const Encoder* capaEnc;
extern const Encoder* portListeningEnc;
extern const Encoder* psync2Enc;
extern const Encoder* psyncEnc;
extern const Encoder* questionMarkEnc;
extern const Encoder* minusOneEnc;
extern const Encoder* starEnc;
extern const std::string stringTypeResp;
extern const std::string streamTypeResp;
extern const std::string listTypeResp;
extern const std::string noneTypeResp;

#endif