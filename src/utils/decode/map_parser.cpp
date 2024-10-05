#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>
#include <cmath>
#include <map>
#include "small_aggregate_parser.h"
#include "simple_data_parser.h"
#include "map_parser.h"
#include "array_parser.h"


MapResp parse_decode_map(const std::string& msg){
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

    int start_pos = end_symbol_pos + 2;
    int count_elements = 0;
    const int msglen = msg.length();
    bool recent_filled_value = true;
    std::vector<std::any> elements_keys;
    std::vector<std::any> elements_values;

    auto fill_keys_values_arrays = [&recent_filled_value, &elements_keys, &elements_values, &count_elements](std::any elt_to_add){
        if (recent_filled_value)
            elements_keys.push_back(elt_to_add);
        else{
            elements_values.push_back(elt_to_add);
            count_elements++;
        }
        recent_filled_value = !recent_filled_value;
    };

    while (end_symbol_pos < msglen - 2 && end_symbol_pos != std::string::npos && count_elements < length){
        switch (msg[start_pos]){
            case '+':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_simple_string(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on simple string", false);
                fill_keys_values_arrays(pair_token.first);
                break;
            }
            case '-':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_error_msg(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on simple error", false);
                fill_keys_values_arrays(pair_token.first);
                break;
            }

            case ':':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_integer(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on integer", false);
                fill_keys_values_arrays(std::get<int>(pair_token.first));
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
                fill_keys_values_arrays(pair_token.first);
                break; 
            }
   
            case '*':
            {
                auto pair_token = parse_decode_array(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
    
                auto pair_array_resp = std::get<ArrayAndInd>(pair_token.first);
                end_symbol_pos = pair_array_resp.second;
                fill_keys_values_arrays(pair_array_resp.first);        
                break;
            }
            case '_':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                const std::string str = msg.substr(start_pos, end_symbol_pos - start_pos + 2);
                if (str.compare("_\r\n") == 0){
                    fill_keys_values_arrays(nullptr);
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
                fill_keys_values_arrays(std::get<bool>(pair_token.first));
                break;
            }
            case ',':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_double(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on double", false);
                fill_keys_values_arrays(std::get<double>(pair_token.first));    
                break;
            }
            case '(':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_big_number(msg.substr(start_pos, end_symbol_pos - start_pos + 2));
                if (!pair_token.second)
                    return std::make_pair("Error on big num", false);
                fill_keys_values_arrays(pair_token.first);    
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
                fill_keys_values_arrays(pair_token.first);     
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
                fill_keys_values_arrays(pair_token.first);
                break;
            }
            case '%':
            {
                auto pair_token = parse_decode_map(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
                auto pair_array_resp = std::get<MapAndInd>(pair_token.first);
                end_symbol_pos = pair_array_resp.second;
                fill_keys_values_arrays(pair_array_resp.first);
                break;
            }
            case '|':
            {
                auto pair_token = parse_decode_map(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
                auto pair_map_resp = std::get<MapAndInd>(pair_token.first);
                end_symbol_pos = pair_map_resp.second;
                fill_keys_values_arrays(pair_map_resp.first);
                break;
            }
            default:
                return std::make_pair("Array format error", false);
        }
        start_pos = end_symbol_pos + 2; 
        if (end_symbol_pos == std::string::npos){
            return std::make_pair("Array format error", false);
        }
        else {
            if (count_elements == length){
                std::map<std::any, std::any> map_data;
                auto itk = elements_keys.begin();
                auto itv = elements_values.begin();
                for (; itk != elements_keys.end() && itv != elements_values.end(); ++itk, ++itv) {
                    map_data[itk] = itv;
                }
                return std::make_pair(std::make_pair(map_data, end_symbol_pos), true);      
            }
            else
                return std::make_pair("Array format error", false);
        }   
    }

}