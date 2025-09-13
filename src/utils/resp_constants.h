#ifndef RESP_CONSTANTS_H
#define RESP_CONSTANTS_H

#include "encode/encoders.h"

extern const std::string okResp;
extern const std::string pongResp;
extern BulkStringEncoder replconfEnc;
extern BulkStringEncoder ackEnc;
extern BulkStringEncoder getackEnc;
extern BulkStringEncoder capaEnc;
extern BulkStringEncoder portListeningEnc;
extern BulkStringEncoder psync2Enc;
extern BulkStringEncoder psyncEnc;
extern BulkStringEncoder questionMarkEnc;
extern BulkStringEncoder minusOneEnc;
extern BulkStringEncoder starEnc;
extern BulkStringEncoder pingEnc;
extern const std::string stringTypeResp;
extern const std::string streamTypeResp;
extern const std::string listTypeResp;
extern const std::string noneTypeResp;

std::vector<Encoder*> stringlist_to_encoderslist(std::vector<std::string> strlist);

#endif