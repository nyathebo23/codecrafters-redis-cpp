#ifndef ENCODERS_H
#define ENCODERS_H

#include <string>
#include <optional>
#include <vector>

class Encoder {
    public:
        Encoder();
        virtual std::string encode() = 0;

};

class BulkStringEncoder: public Encoder {
    private:
        std::string value;
    public:
        BulkStringEncoder(std::string value);
        std::string encode();
};

class IntegerEncoder: public Encoder {
    private:
        int value;
    public:
        IntegerEncoder(int value);
        std::string encode();
};


#endif