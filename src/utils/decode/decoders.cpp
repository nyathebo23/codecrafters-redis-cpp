#include "decoders.h"
#include <iostream>

std::optional<std::string> DecodedResult::get_error() {
    return this->error;
};
DecodedResult::DecodedResult(std::optional<std::string> error) {
    this->error = error;
};

StringDecodeResult::StringDecodeResult(std::string value, std::optional<std::string> error): DecodedResult(std::nullopt) {
    this->value = value;
}

StringDecodeResult::StringDecodeResult(std::string error): DecodedResult(error) {}

std::string StringDecodeResult::as_string() {
    return value;
}

long StringDecodeResult::as_integer() {
    try {
        long val_int = std::stol(this->value);
        return val_int;
    }
    catch (const std::invalid_argument& e) {
        throw CastException(this->value + " can't be converted into integer");
    } 
    catch (const std::out_of_range& e) {
        throw CastException(this->value + " is too big to be converted into integer");
    }        
}

double StringDecodeResult::as_double() {
    try {
        double val_double = std::stod(this->value);
        return val_double;
    }
    catch (const std::invalid_argument& e) {
        throw CastException(this->value + " can't be converted into double");
    } 
    catch (const std::out_of_range& e) {
        throw CastException(this->value + " is too big to be converted into double");
    }      
}

std::vector<DecodedResultPtr> StringDecodeResult::as_array() {
    throw CastException("string cannot be converted into Array");
}


IntDecodeResult::IntDecodeResult(long value): DecodedResult(std::nullopt) {
    this->value = value;
};  
IntDecodeResult::IntDecodeResult(std::string error): DecodedResult(error) {}
std::string IntDecodeResult::as_string() {
    return std::to_string(this->value);
}
long IntDecodeResult::as_integer() {
    return this->value;
}
double IntDecodeResult::as_double() {
    return (double)this->value;
}
std::vector<DecodedResultPtr> IntDecodeResult::as_array() {
    throw CastException("integer cannot be converted into Array");
}

DoubleDecodeResult::DoubleDecodeResult(double value): DecodedResult(std::nullopt) {
    this->value = value;
};  
DoubleDecodeResult::DoubleDecodeResult(std::string error): DecodedResult(error) {}

std::string DoubleDecodeResult::as_string() {
    return std::to_string(this->value);
}
long DoubleDecodeResult::as_integer() {
    return (long)this->value;
}
double DoubleDecodeResult::as_double() {
    return this->value;
}  
std::vector<DecodedResultPtr> DoubleDecodeResult::as_array() {
    throw CastException("double cannot be converted into Array");
}



BooleanDecodeResult::BooleanDecodeResult(bool value): DecodedResult(std::nullopt) {
    this->value = value;
};  
BooleanDecodeResult::BooleanDecodeResult(std::string error): DecodedResult(error) {}

std::string BooleanDecodeResult::as_string() {
    return std::to_string(this->value);
};
long BooleanDecodeResult::as_integer() {
    throw CastException("boolean cannot be converted into integer");
};
double BooleanDecodeResult::as_double() {
    throw CastException("boolean cannot be converted into double");
};
std::vector<DecodedResultPtr> BooleanDecodeResult::as_array() {
    throw CastException("boolean cannot be converted into Array");
}

ArrayDecodeResult::ArrayDecodeResult(std::vector<DecodedResultPtr> value, int charEndInd): DecodedResult(std::nullopt) {
    this->charEndIndex = charEndInd;
    this->value = value;
};  
ArrayDecodeResult::ArrayDecodeResult(std::string error): DecodedResult(error) {}
int ArrayDecodeResult::get_char_end_index() {
    return this->charEndIndex;
}
std::string ArrayDecodeResult::as_string() {
    throw CastException("Array cannot be converted into string");
};
long ArrayDecodeResult::as_integer() {
    throw CastException("Array cannot be converted into int");
};
double ArrayDecodeResult::as_double() {
    throw CastException("Array cannot be converted into double");
};
std::vector<DecodedResultPtr> ArrayDecodeResult::as_array() {
    return this->value;
};
