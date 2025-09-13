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

std::vector<Encoder*> stringlist_to_encoderslist(std::vector<std::string> strlist) {
    std::vector<Encoder*> encoded_list;
    for (std::string item: strlist) {
        BulkStringEncoder enc = BulkStringEncoder(item);
        encoded_list.push_back(&enc);
    }
    return encoded_list;
};


