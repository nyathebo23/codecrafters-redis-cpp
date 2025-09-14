#ifndef SMALL_AGGREGATE_DEC_H // Prévention des inclusions multiples
#define SMALL_AGGREGATE_DEC_H

#include <utility>
#include <string>
#include "decoders.h"

// Déclaration des fonctions
StringDecodeResult* parse_decode_bulk_string(const std::string&);
StringDecodeResult* parse_decode_bulk_error(const std::string& );
StringDecodeResult* parse_decode_verbatim_string(const std::string&);

#endif