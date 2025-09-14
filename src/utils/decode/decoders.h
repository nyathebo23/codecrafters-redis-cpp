#ifndef DECODERS_H
#define DECODERS_H

#include <string>
#include <optional>
#include <vector>
#include <memory>


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
        virtual std::vector<std::shared_ptr<DecodedResult>> asArray() = 0;
};

using DecodedResultPtr = std::shared_ptr<DecodedResult>;

class StringDecodeResult: public DecodedResult {
    private:
        std::string value;
    public:
        StringDecodeResult(std::string value, std::optional<std::string> error);  
        StringDecodeResult(std::string error);
        std::string asString() override;
        long asInteger() override;
        double asDouble() override;
        std::vector<DecodedResultPtr> asArray() override;
};

class IntDecodeResult: public DecodedResult {
    private:
        long value;
    public:
        IntDecodeResult(long value);  
        IntDecodeResult(std::string error);  
        std::string asString() override;
        long asInteger() override;
        double asDouble() override;
        std::vector<DecodedResultPtr> asArray() override;
};

class DoubleDecodeResult: public DecodedResult {
    private:
        double value;
    public:
        DoubleDecodeResult(double value);  
        DoubleDecodeResult(std::string error);  
        std::string asString() override;
        long asInteger() override;
        double asDouble() override;
        std::vector<DecodedResultPtr> asArray() override;
};

class BooleanDecodeResult: public DecodedResult {
    private:
        bool value;
    public:
        BooleanDecodeResult(bool value);  
        BooleanDecodeResult(std::string error);  
        std::string asString() override;
        long asInteger() override;
        double asDouble() override;
        std::vector<DecodedResultPtr> asArray() override;

};

class ArrayDecodeResult: public DecodedResult {
    private:
        std::vector<DecodedResultPtr> value;
        int charEndIndex;
    public:
        ArrayDecodeResult(std::vector<DecodedResultPtr> value, int charEndInd); 
        ArrayDecodeResult(std::string error); 
        int getCharEndIndex();
        std::string asString() override;
        long asInteger() override;
        double asDouble() override;
        std::vector<DecodedResultPtr> asArray() override;
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