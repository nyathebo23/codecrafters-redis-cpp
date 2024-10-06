#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <variant>
#include <any>
#include <cmath>
#include "small_aggregate_parser_dec.h"
#include "simple_data_parser_dec.h"
#include "array_parser_dec.h"
#include "push_parser_dec.h"


ArrayPushResp parse_decode_push(const std::string& msg){
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
                elements.push_back(pair_token.first);
                count_elements++;
                break;
            }

            case '$':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                std::pair<std::string, bool> pair_token;
                if (msg[start_pos+1] == '-')
                    pair_token = parse_decode_bulk_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                else
                    end_symbol_pos = msg.find("\r\n", end_symbol_pos+2);
                    if (!(end_symbol_pos < msglen - 2 && end_symbol_pos != std::string::npos))
                        break;
                    pair_token = parse_decode_bulk_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));                    
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
                end_symbol_pos = pair_array_resp.second;
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
                elements.push_back(pair_token.first);
                count_elements++;
                break;
            }
            case ',':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_double(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on double", false);
                elements.push_back(pair_token.first); 
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
                if (!(end_symbol_pos < msglen - 2 && end_symbol_pos != std::string::npos))
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
                if (!(end_symbol_pos < msglen - 2 && end_symbol_pos != std::string::npos))
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
            //     break;

            default:
                return std::make_pair("Array format error", false);
        }
        start_pos = end_symbol_pos + 2;    
    }

    if (end_symbol_pos == std::string::npos){
        return std::make_pair("Array format error", false);
    }
    else {
        if (count_elements == length && end_symbol_pos == msglen - 2)
            return std::make_pair(elements, true);      
        else
            return std::make_pair("Array format error", false);
    }

}


