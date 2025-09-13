#include "decoders.h"
#include <iostream>

class DecodedResult
{
    private:
        std::optional<std::string> error;
    public:
        std::optional<std::string> getError() {
            return this->getError();
        };
        DecodedResult(std::optional<std::string> error) {
            this->error = error;
        };
        virtual std::string asString() = 0;
        virtual long asInteger() = 0;
        virtual double asDouble() = 0;
        virtual std::vector<DecodedResult&> asArray() = 0;
};

class StringDecodeResult: public DecodedResult {
    private:
        std::string value;
    public:
        StringDecodeResult(std::string value, std::optional<std::string> error): DecodedResult(std::nullopt) {
            this->value = value;
        }  
        StringDecodeResult(std::string error): DecodedResult(error) {}
        std::string asString() override {
            return value;
        }
        long asInteger() override {
            try {
                int val_int = std::stoi(this->value);
            }
            catch (const std::invalid_argument& e) {
                throw CastException(this->value + " can't be converted into integer");
            } 
            catch (const std::out_of_range& e) {
                throw CastException(this->value + " is too big to be converted into integer");
            }        
        }
        double asDouble() override{
            try {
                double val_double = std::stod(this->value);
            }
            catch (const std::invalid_argument& e) {
                throw CastException(this->value + " can't be converted into double");
            } 
            catch (const std::out_of_range& e) {
                throw CastException(this->value + " is too big to be converted into double");
            }      
        }

        std::vector<DecodedResult&> asArray() override {
            throw CastException("string cannot be converted into Array");
        }
};

class IntDecodeResult: public DecodedResult {
    private:
        long value;
    public:
        IntDecodeResult(long value): DecodedResult(std::nullopt) {
            this->value = value;
        };  
        IntDecodeResult(std::string error): DecodedResult(error) {}
        std::string asString() override{
            return std::to_string(this->value);
        }
        long asInteger() override{
            return this->value;
        }
        double asDouble() override{
            return (double)this->value;
        }
        std::vector<DecodedResult&> asArray() override {
            throw CastException("integer cannot be converted into Array");
        }
};

class DoubleDecodeResult: public DecodedResult {
    private:
        double value;
    public:
        DoubleDecodeResult(double value): DecodedResult(std::nullopt) {
            this->value = value;
        };  
        DoubleDecodeResult(std::string error): DecodedResult(error) {}

        std::string asString() override{
            return std::to_string(this->value);
        }
        long asInteger() override{
            return (int)this->value;
        }
        double asDouble() override{
            return this->value;
        }  
        std::vector<DecodedResult&> asArray() override {
            throw CastException("double cannot be converted into Array");
        }
};

class BooleanDecodeResult: public DecodedResult {
    private:
        bool value;
    public:
        BooleanDecodeResult(bool value): DecodedResult(std::nullopt) {
            this->value = value;
        };  
        BooleanDecodeResult(std::string error): DecodedResult(error) {}

        std::string asString() override {
            return std::to_string(this->value);
        };
        long asInteger() override {
            throw CastException("boolean cannot be converted into integer");
        };
        double asDouble() override {
            throw CastException("boolean cannot be converted into double");
        };
        std::vector<DecodedResult&> asArray() override {
            throw CastException("boolean cannot be converted into Array");
        }
};

class ArrayDecodeResult: public DecodedResult {
    private:
        std::vector<DecodedResult&> value;
        int charEndIndex;
    public:
        ArrayDecodeResult(std::vector<DecodedResult&> value, int charEndInd): DecodedResult(std::nullopt) {
            this->charEndIndex = charEndInd;
            this->value = value;
        };  
        ArrayDecodeResult(std::string error): DecodedResult(error) {}
        int getCharEndIndex() {
            return this->charEndIndex;
        }
        std::string asString() {
            throw CastException("Array cannot be converted into string");
        };
        int asInt() {
            throw CastException("Array cannot be converted into int");
        };
        double asDouble() {
            throw CastException("Array cannot be converted into double");
        };
        std::vector<DecodedResult&> asArray() {
            this->value;
        };
};
