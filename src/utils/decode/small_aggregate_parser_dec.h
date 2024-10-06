#ifndef SMALL_AGGREGATE_DEC_H // Prévention des inclusions multiples
#define SMALL_AGGREGATE_DEC_H

#include <utility>
#include <string>

// Déclaration des fonctions
extern std::pair<std::string, bool> parse_decode_bulk_string(const std::string&);
extern std::pair<std::string, bool> parse_decode_bulk_error(const std::string& );
extern std::pair<std::string, bool> parse_decode_verbatim_string(const std::string&);

#endif