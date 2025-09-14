#include "decoders.h"
#include <iostream>

std::optional<std::string> DecodedResult::getError() {
    return this->error;
};
DecodedResult::DecodedResult(std::optional<std::string> error) {
    this->error = error;
};

StringDecodeResult::StringDecodeResult(std::string value, std::optional<std::string> error): DecodedResult(std::nullopt) {
    this->value = value;
}

StringDecodeResult::StringDecodeResult(std::string error): DecodedResult(error) {}

std::string StringDecodeResult::asString() {
    return value;
}

long StringDecodeResult::asInteger() {
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

double StringDecodeResult::asDouble() {
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

std::vector<DecodedResultPtr> StringDecodeResult::asArray() {
    throw CastException("string cannot be converted into Array");
}


IntDecodeResult::IntDecodeResult(long value): DecodedResult(std::nullopt) {
    this->value = value;
};  
IntDecodeResult::IntDecodeResult(std::string error): DecodedResult(error) {}
std::string IntDecodeResult::asString() {
    return std::to_string(this->value);
}
long IntDecodeResult::asInteger() {
    return this->value;
}
double IntDecodeResult::asDouble() {
    return (double)this->value;
}
std::vector<DecodedResultPtr> IntDecodeResult::asArray() {
    throw CastException("integer cannot be converted into Array");
}

DoubleDecodeResult::DoubleDecodeResult(double value): DecodedResult(std::nullopt) {
    this->value = value;
};  
DoubleDecodeResult::DoubleDecodeResult(std::string error): DecodedResult(error) {}

std::string DoubleDecodeResult::asString() {
    return std::to_string(this->value);
}
long DoubleDecodeResult::asInteger() {
    return (long)this->value;
}
double DoubleDecodeResult::asDouble() {
    return this->value;
}  
std::vector<DecodedResultPtr> DoubleDecodeResult::asArray() {
    throw CastException("double cannot be converted into Array");
}



BooleanDecodeResult::BooleanDecodeResult(bool value): DecodedResult(std::nullopt) {
    this->value = value;
};  
BooleanDecodeResult::BooleanDecodeResult(std::string error): DecodedResult(error) {}

std::string BooleanDecodeResult::asString() {
    return std::to_string(this->value);
};
long BooleanDecodeResult::asInteger() {
    throw CastException("boolean cannot be converted into integer");
};
double BooleanDecodeResult::asDouble() {
    throw CastException("boolean cannot be converted into double");
};
std::vector<DecodedResultPtr> BooleanDecodeResult::asArray() {
    throw CastException("boolean cannot be converted into Array");
}

ArrayDecodeResult::ArrayDecodeResult(std::vector<DecodedResultPtr> value, int charEndInd): DecodedResult(std::nullopt) {
    this->charEndIndex = charEndInd;
    this->value = value;
};  
ArrayDecodeResult::ArrayDecodeResult(std::string error): DecodedResult(error) {}
int ArrayDecodeResult::getCharEndIndex() {
    return this->charEndIndex;
}
std::string ArrayDecodeResult::asString() {
    throw CastException("Array cannot be converted into string");
};
long ArrayDecodeResult::asInteger() {
    throw CastException("Array cannot be converted into int");
};
double ArrayDecodeResult::asDouble() {
    throw CastException("Array cannot be converted into double");
};
std::vector<DecodedResultPtr> ArrayDecodeResult::asArray() {
    return this->value;
};
