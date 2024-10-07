#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>
#include <cmath>
#include "small_aggregate_parser_dec.h"
#include "simple_data_parser_dec.h"
#include "map_parser_dec.h"
#include "array_parser_dec.h"

void printArray(std::vector<std::any> arr);

ArrayResp parse_decode_array(const std::string& msg){
    int end_symbol_pos = msg.find("\r\n");
    const std::string numstr = msg.substr(1, end_symbol_pos - 1);
    int length;
    try {
        length = std::stoi(numstr);
    } catch (const std::invalid_argument& e) {
        return std::make_pair("Array length error (is not a number)", false);
    } catch (const std::out_of_range& e) {
        return std::make_pair("Array length out of range (too big)", false);
    }
    std::vector<std::any> elements;
    int start_pos = end_symbol_pos + 2;
    int count_elements = 0;
    const int msglen = msg.length();
    while (end_symbol_pos < msglen - 2 && end_symbol_pos != std::string::npos && count_elements < length){
        switch (msg[start_pos]){
            case '+':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_simple_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on simple string", false);
                elements.push_back(pair_token.first);
                count_elements++;
                break;
            }

            case '-':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_error_msg(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on simple error", false);
                elements.push_back(pair_token.first);
                count_elements++;
                break;
            }
            case ':':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_integer(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on integer", false);

                elements.push_back(std::get<int>(pair_token.first));
                count_elements++;
                break;
            }
            case '$':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                std::pair<std::string, bool> pair_token;
                if (msg[start_pos+1] == '-')
                    pair_token = parse_decode_bulk_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                else {
                    end_symbol_pos = msg.find("\r\n", end_symbol_pos+2);
                    if (end_symbol_pos == std::string::npos)
                        break;
                    pair_token = parse_decode_bulk_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                }
                if (!pair_token.second)
                    return std::make_pair("Error on bulk string", false);
                elements.push_back(pair_token.first);
                count_elements++;
                break; 
            }
            case '*':
            {
                auto pair_token = parse_decode_array(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
    
                auto pair_array_resp = std::get<ArrayAndInd>(pair_token.first);
                end_symbol_pos = start_pos + pair_array_resp.second;
                elements.push_back(pair_array_resp.first);
                count_elements++;
                break;
            }
            case '_':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                const std::string str = msg.substr(start_pos, end_symbol_pos - start_pos + 2);
                if (str.compare("_\r\n") == 0){
                    elements.push_back(nullptr);
                    count_elements++;
                }
                else
                    return std::make_pair("Error on null", false);
                break;
            }
            case '#':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_boolean(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on boolean", false);
                elements.push_back(std::get<bool>(pair_token.first));
                count_elements++;
                break;
            }
            case ',':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_double(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on double", false);
                elements.push_back(std::get<double>(pair_token.first)); 
                count_elements++;           
                break;
            }
            case '(':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_big_number(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on big num", false);
                elements.push_back(pair_token.first);     
                count_elements++;          
                break;
            }
            case '!':
            {
                end_symbol_pos = msg.find("\r\n", msg.find("\r\n", start_pos)+1);
                if (end_symbol_pos == std::string::npos)
                    break;
                auto pair_token = parse_decode_bulk_error(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on bulk error", false);
                elements.push_back(pair_token.first);    
                count_elements++;           
                break;
            }
            case '=':
            {
                end_symbol_pos = msg.find("\r\n", msg.find("\r\n", start_pos)+1);
                if (end_symbol_pos == std::string::npos)
                    break;
                auto pair_token = parse_decode_verbatim_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on verbatim string", false);
                elements.push_back(pair_token.first); 
                count_elements++;    
                break;
            }
            // case '%':
            // {
            //     auto pair_token = parse_decode_map(msg.substr(start_pos));
            //     if (!pair_token.second)
            //         return std::make_pair("Array format error", false);
            //     auto pair_array_resp = std::get<MapAndInd>(pair_token.first);
            //     end_symbol_pos = pair_array_resp.second;
            //     elements.push_back(pair_array_resp.first);
            //     count_elements++;    
            //     break;
            // }
            // case '|':
            // {
            //     auto pair_token = parse_decode_map(msg.substr(start_pos));
            //     if (!pair_token.second)
            //         return std::make_pair("Array format error", false);
            //     auto pair_map_resp = std::get<MapAndInd>(pair_token.first);
            //     end_symbol_pos = pair_map_resp.second;
            //     elements.push_back(pair_map_resp.first);
            //     count_elements++;   
            // }
            default:
                return std::make_pair("Array format error", false);
        }
        start_pos = end_symbol_pos + 2; 

    }

    if (end_symbol_pos == std::string::npos){
        return std::make_pair("Array format error", false);
    }
    else {
        if (count_elements == length)
            return std::make_pair(std::make_pair(elements, end_symbol_pos), true);      
        else
            return std::make_pair("Array format error", false);
    }

}

// int main(int argc, char **argv) {
//     const std::string str = "*3\r\n*3\r\n:50\r\n+retypA\r\n=8\r\ntxt:ECHO\r\n$3\r\nhey\r\n*2\r\n*3\r\n,25.55e10\r\n$6\r\nECHOpA\r\n$6\r\neleven\r\n#t\r\n";
//     ArrayResp arrresp = parse_decode_array(str);
//     auto arr = std::get<ArrayAndInd>(arrresp.first);
//     auto vals = arr.first;
//     for (int i = 0; i < argc; i++){
//         std::cout << argv[i] << std::endl;
//     }
//     printArray(vals);
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