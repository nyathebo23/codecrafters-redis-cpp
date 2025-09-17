#ifndef RESP_CONSTANTS_H
#define RESP_CONSTANTS_H

#include "encode/encoders.h"

extern const std::string NULL_BULK_STRING;
extern const std::string NULL_ARRAY;
extern const std::string SUBSCRIBE_ENC;
extern const std::string UNSUBSCRIBE_ENC;
extern const std::string PING_SUBS_MODE_RESP;
extern const std::string OK_RESP;
extern const std::string PONG_RESP;
extern const std::string QUEUE_RESP;
extern const std::string REPLCONF_GETACK_CMD;
extern const std::string REPLCONF_ACK_PART_ENC;
extern const std::string REPLCONF_MSG2_ENC;
extern const std::string REPLCONF_MSG1_PART_ENC;
extern const std::string PSYNC_MSG_ENC;
extern const std::string PING_AS_LIST_ENC;
extern const std::string STRING_TYPE_RESP;
extern const std::string STREAM_TYPE_RESP;
extern const std::string LIST_TYPE_RESP;
extern const std::string NONE_TYPE_RESP;

#endif