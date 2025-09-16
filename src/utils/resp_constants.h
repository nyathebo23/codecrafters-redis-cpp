#ifndef RESP_CONSTANTS_H
#define RESP_CONSTANTS_H

#include "encode/encoders.h"

extern const std::string NULL_BULK_STRING;
extern const std::string NULL_ARRAY;
extern const std::string SUBSCRIBE_ENC;
extern const std::string SUBSCRIBE_ENC;
extern const std::string PING_SUBS_MODE_RESP;
extern const std::string okResp;
extern const std::string pongResp;
extern const std::string queueResp;
extern const std::string replconfGetackCmd;
extern const std::string replconfAckPartEnc;
extern const std::string replconfMsg2Enc;
extern const std::string replconfMsg1PartEnc;
extern const std::string psyncMsgEnc;
extern const std::string pingAsListEnc;
extern const std::string stringTypeResp;
extern const std::string streamTypeResp;
extern const std::string listTypeResp;
extern const std::string noneTypeResp;

#endif