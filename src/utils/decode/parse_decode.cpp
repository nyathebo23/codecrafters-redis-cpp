#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>
#include <cmath>
#include <map>
#include <unordered_set>


std::string extractSubstr(const std::string str, int startPos, int endPos) {
    return str.substr(startPos, endPos - startPos + 1);
}

bool detect_wrong_char_in_substring(const std::string& str){
    if (str.find('\r') != std::string::npos || str.find('\n') != std::string::npos)
        return true;
    return false;
};

bool is_digits(const std::string& str) {
    for (char ch : str) {
        int v = ch; // ASCII Val converted
        if (!(ch >= 48 && ch <= 57)) {
            return false;
        }
    }
    return true;
}

std::pair<std::string, bool> parse_decode_simple_string(const std::string& msg){
    if (detect_wrong_char_in_substring(msg))
        return std::make_pair("", false);
    return std::make_pair(extractSubstr(msg, 1, msg.length() - 3), true);
}

std::pair<std::string, bool> parse_decode_error_msg(const std::string& msg){
    if (detect_wrong_char_in_substring(msg))
        return std::make_pair("", false);
    return std::make_pair(extractSubstr(msg, 1, msg.length() - 3), true);
}

std::pair<std::variant<int, std::string>, bool> parse_decode_integer(const std::string& msg){
    const std::string strnum = extractSubstr(msg, 1, msg.length() - 3);
    try {
        int number = std::stoi(strnum);
        return std::make_pair(number, true);
    } catch (const std::invalid_argument& e) {
        return std::make_pair(e.what(), false);
    } catch (const std::out_of_range& e) {
        return std::make_pair(e.what(), false);
    }
}

std::pair<std::string, bool> parse_decode_bulk_string(const std::string& msg){
    if (msg.substr(1, 2).compare("-1") == 0)
        if (msg.compare("$-1\r\n") != 0)
            return std::make_pair("Error", false);
        else
            return std::make_pair("", true);

    const std::size_t endPosr = msg.find('\r');
    const std::size_t endPosr2 = msg.find('\r', endPosr+1);
    if (endPosr == std::string::npos || endPosr2 == std::string::npos || msg[endPosr+1] != '\n' || 
        msg[endPosr2+1] != '\n' || endPosr2 + 2 != msg.length()){
        return std::make_pair("Error", false);
    }
    const std::string strnum =  msg.substr(1, endPosr - 1);
    const std:: string text = msg.substr(endPosr+2, endPosr2 - endPosr);
    if (detect_wrong_char_in_substring(msg))
        return std::make_pair("", false);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return std::make_pair(text, false);
        else
            return std::make_pair(text, true);
    } catch (const std::invalid_argument& e) {
            return std::make_pair("Text length error", false);
    } catch (const std::out_of_range& e) {
            return std::make_pair("Text length out of range", false);
    }
}


std::pair<std::string, bool> parse_decode_verbatim_string(const std::string& msg){

    const std::size_t endPosr = msg.find('\r');
    const std::size_t endPosr2 = msg.find('\r', endPosr+1);
    if (endPosr == std::string::npos || endPosr2 == std::string::npos || msg[endPosr+1] != '\n' || 
        msg[endPosr2+1] != '\n' || endPosr2 + 2 != msg.length()){
        return std::make_pair("Error", false);
    }

    const std::string strnum =  msg.substr(1, endPosr - 1);
    const std:: string text = msg.substr(endPosr+2, endPosr2 - endPosr);
    
    if (detect_wrong_char_in_substring(msg))
        return std::make_pair("", false);
    const int pos_separator = text.find(':');
    const std::string encoding = text.substr(0, pos_separator);
    if (encoding.length() != 3 && pos_separator != std::string::npos)
        return std::make_pair("Format encoding Error", false);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return std::make_pair(text, false);
        else
            return std::make_pair(text, true);
    } catch (const std::invalid_argument& e) {
        return std::make_pair("Text length error", false);
    } catch (const std::out_of_range& e) {
        return std::make_pair("Text length out of range", false);
    }
}


std::pair<std::variant<bool, std::string>, bool> parse_decode_boolean(const std::string& msg){
    if (msg.compare("#t\r\n") == 0)
        return std::make_pair(true, true);
    else if (msg.compare("#f\r\n") == 0)
        return std::make_pair(true, true);
    else return std::make_pair("Error", false);
}

std::pair<std::variant<double, std::string>, bool> parse_decode_double(std::string msg){
    std::size_t posE = msg.find('e');
    if (posE == std::string::npos)
        posE = msg.find('E');
    try {
        if (posE == std::string::npos){
            const std::string strnum = msg.substr(1, msg.length() - 3);
            double number = std::stod(strnum);
            return std::make_pair(number, true);
        }
        else {
            const std::string exp = extractSubstr(msg, posE + 1, msg.length() - 3);
            const std::string strnum = msg.substr(1, posE - 1);
            int expint = std::stoi(exp);
            double number = std::stod(strnum) * std::pow(10, expint);
            return std::make_pair(number, true);
        }
    } catch (const std::invalid_argument& e) {
        return std::make_pair("Error", false);
    } catch (const std::out_of_range& e) {
        return std::make_pair("Error", false);
    }
}

std::pair<std::string, bool> parse_decode_big_number(const std::string& msg){
    const std::string num = msg.substr(1, msg.length() - 3);
    if (detect_wrong_char_in_substring(num) || !is_digits(num))
        return std::make_pair("Error", false);
    return std::make_pair(num, true); 
}

std::pair<std::string, bool> parse_decode_bulk_error(std::string msg){

    const std::size_t endPosr = msg.find('\r');
    const std::size_t endPosr2 = msg.find('\r', endPosr+1);

    if (endPosr == std::string::npos || endPosr2 == std::string::npos || msg[endPosr+1] != '\n' || 
        msg[endPosr2+1] != '\n' || endPosr2 + 2 != msg.length()){
        return std::make_pair("Error", false);
    }
    const std::string strnum =  msg.substr(1, endPosr - 1);
    const std:: string text = msg.substr(endPosr+2, endPosr2 - endPosr);
    if (detect_wrong_char_in_substring(msg))
        return std::make_pair("", false);
    try {
        int number = std::stoi(strnum);
        if (number != text.length())
            return std::make_pair(text, false);
        else
            return std::make_pair(text, true);
    } catch (const std::invalid_argument& e) {
        return std::make_pair("Error", false);
    } catch (const std::out_of_range& e) {
        return std::make_pair("Error", false);
    }
}

using ArrayAndInd = std::pair<std::vector<std::any>, int>;
using ArrayResp = std::pair<std::variant<ArrayAndInd, std::string>, bool>;
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
                auto pair_token = parse_decode_simple_string(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on simple string", false);
                elements.push_back(pair_token.first);
                count_elements++;
                break;
            }

            case '-':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_error_msg(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on simple error", false);
                elements.push_back(pair_token.first);
                count_elements++;
                break;
            }

            case ':':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_integer(extractSubstr(msg, start_pos, end_symbol_pos+1));
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
                    pair_token = parse_decode_bulk_string(extractSubstr(msg, start_pos, end_symbol_pos+1));
                else
                    end_symbol_pos = msg.find("\r\n", end_symbol_pos+2);
                    if (!(end_symbol_pos < msglen - 2 && end_symbol_pos != std::string::npos))
                        break;
                    pair_token = parse_decode_bulk_string(extractSubstr(msg, start_pos, end_symbol_pos+1));                    
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
                const std::string str = extractSubstr(msg, start_pos, end_symbol_pos+1);
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
                auto pair_token = parse_decode_boolean(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on boolean", false);
                elements.push_back(pair_token.first);
                count_elements++;
                break;
            }
            case ',':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_double(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on double", false);
                elements.push_back(pair_token.first); 
                count_elements++;           
                break;
            }
            case '(':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_big_number(extractSubstr(msg, start_pos, end_symbol_pos+1));
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
                auto pair_token = parse_decode_bulk_error(extractSubstr(msg, start_pos, end_symbol_pos+1));
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
                auto pair_token = parse_decode_verbatim_string(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on verbatim string", false);
                elements.push_back(pair_token.first); 
                count_elements++;    
                break;
            }
            case '%':
            {
                auto pair_token = parse_decode_map(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
                auto pair_array_resp = std::get<MapAndInd>(pair_token.first);
                end_symbol_pos = pair_array_resp.second;
                elements.push_back(pair_array_resp.first);
                count_elements++;    
                break;
            }
            case '|':
            {
                auto pair_token = parse_decode_map(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
                auto pair_map_resp = std::get<MapAndInd>(pair_token.first);
                end_symbol_pos = pair_map_resp.second;
                elements.push_back(pair_map_resp.first);
                count_elements++;   
            }
                break;
            case '~':
            {
                auto pair_token = parse_decode_unordered_set(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
                auto pair_set_resp = std::get<SetAndInd>(pair_token.first);
                end_symbol_pos = pair_set_resp.second;
                elements.push_back(pair_set_resp.first);
                count_elements++; 
            }
                break;
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

using MapAndInd = std::pair<std::map<std::any, std::any>, int>;
using MapResp = std::pair<std::variant<MapAndInd, std::string>, bool>;
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
    std::map<std::any, std::any> elts_map;
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
                auto pair_token = parse_decode_simple_string(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on simple string", false);
                fill_keys_values_arrays(pair_token.first);
                break;
            }
            case '-':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_error_msg(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on simple error", false);
                fill_keys_values_arrays(pair_token.first);
                break;
            }

            case ':':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_integer(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on integer", false);
                fill_keys_values_arrays(pair_token.first);
                break;
            }

            case '$':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                std::pair<std::string, bool> pair_token;
                if (msg[start_pos+1] == '-')
                    pair_token = parse_decode_bulk_string(extractSubstr(msg, start_pos, end_symbol_pos+1));
                else
                    end_symbol_pos = msg.find("\r\n", end_symbol_pos+2);
                    if (!(end_symbol_pos < msglen - 2 && end_symbol_pos != std::string::npos))
                        break;
                    pair_token = parse_decode_bulk_string(extractSubstr(msg, start_pos, end_symbol_pos+1));                    
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
                const std::string str = extractSubstr(msg, start_pos, end_symbol_pos+1);
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
                auto pair_token = parse_decode_boolean(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on boolean", false);
                fill_keys_values_arrays(pair_token.first);
                break;
            }
            case ',':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_double(extractSubstr(msg, start_pos, end_symbol_pos+1));
                if (!pair_token.second)
                    return std::make_pair("Error on double", false);
                fill_keys_values_arrays(pair_token.first);    
                break;
            }
            case '(':
            {
                end_symbol_pos = msg.find("\r\n", start_pos);
                auto pair_token = parse_decode_big_number(extractSubstr(msg, start_pos, end_symbol_pos+1));
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
                auto pair_token = parse_decode_bulk_error(extractSubstr(msg, start_pos, end_symbol_pos+1));
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
                auto pair_token = parse_decode_verbatim_string(extractSubstr(msg, start_pos, end_symbol_pos+1));
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
                break;
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
            case '~':
            {
                auto pair_token = parse_decode_unordered_set(msg.substr(start_pos));
                if (!pair_token.second)
                    return std::make_pair("Array format error", false);
                auto pair_set_resp = std::get<SetAndInd>(pair_token.first);
                end_symbol_pos = pair_set_resp.second;
                fill_keys_values_arrays(pair_set_resp.first); 
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
                    map_data[itk] = map_data[itv];
                }
                return std::make_pair(std::make_pair(map_data, end_symbol_pos), true);      
            }
            else
                return std::make_pair("Array format error", false);
        }   
    }

}

using SetAndInd = std::pair<std::unordered_set<std::any>, int>;
using SetResp = std::pair<std::variant<SetAndInd, std::string>, bool>;
SetResp parse_decode_unordered_set(const std::string& msg){
    auto pair_token = parse_decode_array(msg);
    if (!pair_token.second)
        return std::make_pair("Array format error", false);

    auto pair_array_resp = std::get<ArrayAndInd>(pair_token.first);
    auto array = pair_array_resp.first;
    int end_symbol_pos = pair_array_resp.second;

    std::unordered_set<std::any> set_datas(std::begin(array), std::end(array));
    if (set_datas.size() != array.size())
        return std::make_pair("Array format error", false);    

    return std::make_pair(std::make_pair(set_datas, end_symbol_pos), true);      

}

