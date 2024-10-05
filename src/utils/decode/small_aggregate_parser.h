#ifndef SMALL_AGGREGATE_H // Prévention des inclusions multiples
#define SMALL_AGGREGATE_H

#include <utility>
#include <string>

// Déclaration des fonctions
std::pair<std::string, bool> parse_decode_bulk_string(const std::string&);
std::pair<std::string, bool> parse_decode_bulk_error(const std::string& );
std::pair<std::string, bool> parse_decode_verbatim_string(const std::string&);

#endif