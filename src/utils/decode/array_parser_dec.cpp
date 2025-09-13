#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>
#include <cmath>
#include "small_aggregate_parser_dec.h"
#include "simple_data_parser_dec.h"
#include "array_parser_dec.h"
#include "decoders.h"

ArrayDecodeResult parse_decode_array(const std::string& msg){
    int end_symbol_pos = msg.find("\r\n");
    const std::string numstr = msg.substr(1, end_symbol_pos - 1);
    int length;
    try {
        length = std::stoi(numstr);
    } catch (const std::invalid_argument& e) {
        return  ArrayDecodeResult("Array length error (is not a number)");
    } catch (const std::out_of_range& e) {
        return ArrayDecodeResult("Array length out of range (too big)");
    }
    std::vector<DecodedResult*> elements;
    int start_pos = end_symbol_pos + 2;
    int count_elements = 0;
    const int msglen = msg.length();
    while (end_symbol_pos < msglen - 2 &&  count_elements < length){
        if ((end_symbol_pos = msg.find("\r\n", start_pos)) == std::string::npos)
            return ArrayDecodeResult("End not found");
        switch (msg[start_pos]){
            case ':':
            {
                auto decodedVal = parse_decode_integer(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (decodedVal.getError())
                    return ArrayDecodeResult("Error when parsing integer at index "+ elements.size());
                elements.push_back(&decodedVal);
                count_elements++;
                break;
            }
            case '$':
            {
                StringDecodeResult decodedVal("");
                if (msg[start_pos+1] == '-')
                    decodedVal = parse_decode_bulk_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                else {
                    end_symbol_pos = msg.find("\r\n", end_symbol_pos+2);
                    if (end_symbol_pos == std::string::npos)
                        return ArrayDecodeResult("End not found");
                    decodedVal = parse_decode_bulk_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                }
                if (decodedVal.getError())
                    return ArrayDecodeResult("Error when parsing bulk string at index "+ elements.size());
                elements.push_back(&decodedVal);
                count_elements++;
                break; 
            }
            // case '*':
            // {
            //     auto decodedVal = parse_decode_array(msg.substr(start_pos));
            //     if (decodedVal.getError())
            //         return ArrayDecodeResult("Array format error");
            //     end_symbol_pos = start_pos + decodedVal.getCharEndIndex();
            //     elements.push_back(&decodedVal);
            //     count_elements++;
            //     break;
            // }
            // case '_':
            // {
            //     const std::string str = msg.substr(start_pos, end_symbol_pos - start_pos + 2);
            //     if (str.compare("_\r\n") == 0){
            //         elements.push_back(&nullptr);
            //         count_elements++;
            //     }
            //     else
            //         return std::make_pair("Error on null", false);
            //     break;
            // }
            case '#':
            {
                auto decodedVal = parse_decode_boolean(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (decodedVal.getError())
                    return ArrayDecodeResult("Error when parsing boolean at index "+ elements.size());
                elements.push_back(&decodedVal);
                count_elements++;
                break;
            }
            case ',':
            {
                auto decodedVal = parse_decode_double(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (decodedVal.getError())
                    return ArrayDecodeResult("Error when parsing double at index "+ elements.size());
                elements.push_back(&decodedVal); 
                count_elements++;           
                break;
            }
            case '(':
            {
                auto decodedVal = parse_decode_big_number(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (decodedVal.getError())
                    return ArrayDecodeResult("Error when parsing big num at index "+ elements.size());
                elements.push_back(&decodedVal);     
                count_elements++;          
                break;
            }
            // case '!':
            // {
            //     end_symbol_pos = msg.find("\r\n", end_symbol_pos + 1);
            //     if (end_symbol_pos == std::string::npos)
            //         return std::make_pair(std::make_pair(elements, start_pos), false);
            //     auto decodedVal = parse_decode_bulk_error(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
            //     if (!decodedVal.second)
            //         return std::make_pair("Error on bulk error", false);
            //     elements.push_back(&decodedVal.first);    
            //     count_elements++;           
            //     break;
            // }
            // case '=':
            // {
            //     end_symbol_pos = msg.find("\r\n", end_symbol_pos + 1);
            //     if (end_symbol_pos == std::string::npos)
            //         return std::make_pair(std::make_pair(elements, start_pos), false);
            //     auto decodedVal = parse_decode_verbatim_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
            //     if (!decodedVal.second)
            //         return std::make_pair("Error on verbatim string", false);
            //     elements.push_back(decodedVal.first); 
            //     count_elements++;    
            //     break;
            // }
            default:
                return ArrayDecodeResult("Array format error");
        }
        start_pos = end_symbol_pos + 2; 
    }

    if (count_elements == length)
        return ArrayDecodeResult("End not found");
    else
        return ArrayDecodeResult(elements, start_pos);
}

// int main(int argc, char **argv) {
//     const std::string str = "*2\r\n$4\r\nKEYS\r\n$1\r\n*\r\n";
//     ArrayResp arrresp = parse_decode_array(str);
//     auto arr = std::get<ArrayAndInd>(arrresp.first);
//     auto vals = arr.first;

//     printArray(vals);
//     bool b = std::any_cast<std::string>(vals[1]) == "*";
//     std::cout << b << true;
//     return 0;
// }

// void printArray(std::vector<std::any> arr){

//     for (const auto& elt: arr){
//         if (elt.type() == typeid(int)) {
//             std::cout << std::any_cast<int>(elt) << '\n';
//         } else if (elt.type() == typeid(double)) {
//             std::cout << std::any_cast<double>(elt) << '\n';
//         } else if (elt.type() == typeid(std::string)) {
//             std::cout << std::any_cast<std::string>(elt) << '\n';
//         }
//         else if (elt.type() == typeid(bool)) {
//             std::cout << std::any_cast<bool>(elt) << '\n';
//         }
//         else if (elt.type() == typeid(std::vector<std::any>)) {
//             printArray(std::any_cast<std::vector<std::any>>(elt));
//         }
//     }
// }