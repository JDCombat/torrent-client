#ifndef BENCODE_H
#define BENCODE_H

#include <any>
#include <cstdint>
#include <cstring>
#include <format>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include "nlohmann/json.hpp"


namespace bencode {

    std::string decode_string(const std::string &s, uint32_t* pos);
    std::string decode_int(const std::string& s, uint32_t* pos);
    std::string decode_array(const std::string &s, uint32_t* pos);
    std::string decode_dictionary(const std::string &s, uint32_t* pos, bool isHex = false);
    std::string to_hex(std::string &s);
    std::string encode_dictionary(const nlohmann::json &j);
    std::string encode_array(const nlohmann::json &j);
    std::string encode_string(const nlohmann::json &j);
    std::string encode_int(const nlohmann::json &j);
    std::string encode_hex_string(const nlohmann::json &j);

    inline std::string decode_string(const std::string &s, uint32_t* pos) {
        std::string str_length_str = s.substr(*pos, s.find(':', *pos+1) - *pos);
        *pos += str_length_str.length()+1;
        std::string str = s.substr(*pos, stoi(str_length_str));
        *pos += stoi(str_length_str);
        return str;
    }

    std::string decode_int(const std::string& s, uint32_t* pos) {
        const uint32_t e_pos = s.find('e',*pos+1);
        std::string str = s.substr(*pos+1, e_pos-(*pos)-1);
        if (str == "-0") {
            throw std::runtime_error("Error in file (parsing int '-0')");
        }
        if (str.length() > 1 && str[0] == '0') {
            throw std::runtime_error("Error in file (parsing int '0x')");
        }
        *pos = e_pos+1;
        return str;
    }
    std::string decode_array(const std::string &s, uint32_t* pos) {
        std::string str = "[";
        while (s[*pos] != 'e') {
            switch (s[*pos]) {
                case 'i':
                    str += decode_int(s, pos) + ',';
                    break;
                case 'l':
                    *pos += 1;
                    str += decode_array(s, pos) + ',';
                    break;
                case 'd':
                    ++(*pos);
                    str += decode_dictionary(s, pos) + ',';
                    break;
                default:
                    str += '"' + decode_string(s, pos) + '"' + ',';
                    break;
            }
        }
        str = str.substr(0, str.length()-1);
        if (str.empty()) {
            str = "[";
        }
        str += "]";
        *pos +=1;
        return str;
    }

    std::string decode_dictionary(const std::string &s, uint32_t* pos, bool isHex) {
        std::string str = "{";
        while (s[*pos] != 'e') {
            std::string key = decode_string(s, pos);
            if (isHex) {
                key = to_hex(key);
            }
            if (key.empty()) {
                key = "\"\"";
            }
            std::string value;

            // std::cout << "klucz " << key << std::endl;
            // std::cout << *pos << std::endl;

            switch (s[*pos]) {
                case 'i':
                    value = decode_int(s, pos);
                    break;
                case 'd':
                    ++(*pos);
                    if (key == "piece layers") {
                        value = decode_dictionary(s, pos, true);
                        break;
                    }
                    value = decode_dictionary(s, pos);
                    break;
                case 'l':
                    *pos += 1;
                    value = decode_array(s, pos);
                    break;
                default:
                    std::string tmpstr = decode_string(s, pos);
                    if (key == "pieces" || key == "pieces root" || isHex || key == "peers") {
                        value = '"' + to_hex(tmpstr) + '"';
                    }
                    else {
                        value = '"' + tmpstr + '"';
                    }
                    break;
            }
            str += '"' + key + '"' + ':' + value + ",";
        }
        *pos += 1;
        str = str.substr(0, str.length()-1)+"}";
        return str;
    }

    nlohmann::json decode_file(const std::string &s) {
        if (s[0] != 'd') {
            throw std::invalid_argument("Error in file (expected 'd' on index 0)");
        }
        if (s[s.size()-1] != 'e') {
            throw std::invalid_argument("Error in file (expected 'e' on last index)");
        }
        uint32_t pos = 1;


        std::string decoded = decode_dictionary(s, &pos);

        // std::cout << decoded << std::endl;

        return nlohmann::json::parse(decoded);

    }

    std::string to_hex(std::string &s)
    {
        std::stringstream ss;
        auto ptr = reinterpret_cast<uint8_t*>(s.data());
        ss << std::hex << std::setfill('0');
        for (size_t i = 0; i < s.size(); i++) {
            ss << std::setw(2) << static_cast<int>(ptr[i]);
        }
        return ss.str();
    }

    std::string encode_dictionary(const nlohmann::json &j) {
        std::stringstream ss;
        ss << 'd';
        for (auto& [key, value] : j.items()) {
            ss << key.length() << ':' << key;
            if (value.is_array()) {
                ss << encode_array(value);
            }
            else if (value.is_number()) {
                ss << encode_int(value);
            }
            else if (value.is_object()) {
                ss << encode_dictionary(value);
            }
            else if (value.is_string()) {
                if (key == "pieces" || key == "pieces root" || key == "peers") {
                    ss << encode_hex_string(value);
                }
                else {
                    ss << encode_string(value);
                }
            }
        }
        ss << 'e';
        return ss.str();
    }

    std::string encode_array(const nlohmann::json &j) {
        std::stringstream ss;
        ss << 'l';
        for (auto& value : j) {
            if (value.is_array()) {
                ss << encode_array(value);
            }
            else if (value.is_number()) {
                ss << encode_int(value);
            }
            else if (value.is_object()) {
                ss << encode_dictionary(value);
            }
            else {
                ss << encode_string(value);
            }

        }

        ss << 'e';
        return ss.str();
    }

    std::string encode_string(const nlohmann::json &j) {
        std::stringstream ss;
        ss << j.get<std::string>().length() << ':' << j.get<std::string>();
        return ss.str();
    }
    std::string encode_int(const nlohmann::json &j) {
        std::stringstream ss;
        ss << 'i' << j.get<int64_t>() << 'e';
        return ss.str();
    }

    std::string encode_hex_string(const nlohmann::json &j) {
        std::stringstream ss;
        const std::string stringHex = j.get<std::string>();
        std::string bytesHex;
        for (int i = 0; i < stringHex.length() - 1; i+=2) {
            bytesHex += static_cast<char>(std::stoi(stringHex.substr(i, 2), nullptr, 16));
        }
        ss << bytesHex.length() << ':' << bytesHex;
        return ss.str();
    }



}


#endif //BENCODE_H
