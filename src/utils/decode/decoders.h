#ifndef DECODERS_H
#define DECODERS_H

#include <string>
#include <optional>
#include <vector>

class DecodedResult
{
    private:
        std::optional<std::string> error;
    public:
        std::optional<std::string> getError();
        DecodedResult(std::optional<std::string> error);
        virtual std::string asString() = 0;
        virtual long asInteger() = 0;
        virtual double asDouble() = 0;
        virtual std::vector<DecodedResult&> asArray() = 0;
};

class StringDecodeResult: public DecodedResult {
    private:
        std::string value;
    public:
        StringDecodeResult(std::string value, std::optional<std::string> error);  
        StringDecodeResult(std::string error);
        std::string asString();
        long asInteger();
        double asDouble();
        std::vector<DecodedResult&> asArray();
};

class IntDecodeResult: public DecodedResult {
    private:
        long value;
    public:
        IntDecodeResult(long value);  
        IntDecodeResult(std::string error);  
        std::string asString();
        long asInteger();
        double asDouble();
        std::vector<DecodedResult&> asArray();
};

class DoubleDecodeResult: public DecodedResult {
    private:
        double value;
    public:
        DoubleDecodeResult(double value);  
        DoubleDecodeResult(std::string error);  
        std::string asString();
        long asInteger();
        double asDouble();
        std::vector<DecodedResult&> asArray();
};

class BooleanDecodeResult: public DecodedResult {
    private:
        bool value;
    public:
        BooleanDecodeResult(bool value);  
        BooleanDecodeResult(std::string error);  
        std::string asString();
        long asInteger();
        double asDouble();
        std::vector<DecodedResult&> asArray();

};

class ArrayDecodeResult: public DecodedResult {
    private:
        std::vector<DecodedResult&> value;
        int charEndIndex;
    public:
        ArrayDecodeResult(std::vector<DecodedResult&> value, int charEndInd); 
        ArrayDecodeResult(std::string error); 
        int getCharEndIndex();
        std::string asString();
        long asInteger();
        double asDouble();
        std::vector<DecodedResult&> asArray();
};



class CastException : public std::exception {
public:
    explicit CastException(const std::string& message) : _message(message) {}

    const char* what() const noexcept override {
        return _message.c_str();
    }

private:
    std::string _message;
};

#endif