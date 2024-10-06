#ifndef SMALL_AGGREGATE_ENC_H // Prévention des inclusions multiples
#define SMALL_AGGREGATE_ENC_H

#include <utility>
#include <string>

// Déclaration des fonctions
extern std::string parse_encode_bulk_string(const std::string&);
extern std::string parse_encode_bulk_error(const std::string& );
extern std::string parse_encode_verbatim_string(const std::string&);

#endif