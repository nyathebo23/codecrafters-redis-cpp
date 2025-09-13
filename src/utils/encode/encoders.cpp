#include "encoders.h"
#include "small_aggregate_parser_enc.h"
#include "simple_data_parser_enc.h"


Encoder::Encoder(){}

BulkStringEncoder::BulkStringEncoder(std::string value): Encoder() {
    this->value = value;
}
std::string BulkStringEncoder::encode()  {
    return parse_encode_bulk_string(this->value);
}


IntegerEncoder::IntegerEncoder(long value): Encoder() {
    this->value = value;
}
std::string IntegerEncoder::encode() {
    return parse_encode_integer(this->value); 
};




