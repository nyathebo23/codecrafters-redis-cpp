#ifndef SMALL_AGGREGATE_H // Prévention des inclusions multiples
#define SMALL_AGGREGATE_H

#include <utility>
#include <string>

// Déclaration des fonctions
std::string parse_encode_bulk_string(const std::string&);
std::string parse_encode_bulk_error(const std::string& );
std::string parse_encode_verbatim_string(const std::string&);

#endif