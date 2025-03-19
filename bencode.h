#ifndef BENCODE_H
#define BENCODE_H

#include <any>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <vector>


namespace bencode {
    inline std::string decode_string(std::string s, uint32_t* pos) {
        std::string str_length_str = s.substr(*pos, s.find(':', *pos+1) - *pos);
        *pos += str_length_str.length()+1;
        std::string str = s.substr(*pos, stoi(str_length_str));
        *pos += stoi(str_length_str);
        return str;
    }

    std::string decode_int(std::string s, uint32_t* pos) {
        uint8_t e_pos = s.find('e',*pos+1);
        std::string str = s.substr(*pos+1, e_pos-*pos-1);
        if (str == "-0") {
            throw std::runtime_error("Error in file (parsing int '-0')");
        }
        if (str.length() > 1 && str[0] == '0') {
            throw std::runtime_error("Error in file (parsing int '0x')");
        }
        *pos = e_pos+1;
        return str;
    }
    std::string decode_array(std::string s, uint32_t* pos) {
        (*pos)++;
        std::string str = "[";
        while (s[*pos] != 'e') {
            str+=decode_string(s, pos) + ",";
        }
        str = str.substr(0, str.length()-2)+"]";
        return str;
    }

    std::string decode_dictionary(std::string s, uint32_t* pos) {
        std::string str = "{";
        while (s[*pos] != 'e') {
            std::string key = decode_string(s, pos);
            std::string value;
            std::cout << "klucz " << key << std::endl;
            std::cout << *pos << std::endl;
            if (std::__format::__is_digit(s[*pos])) {
                value = decode_string(s, pos);
            }
            else if (s[*pos] == 'i') {
                value = decode_int(s, pos);
            }
            else if (s[*pos] == 'l') {
                value = decode_array(s, pos);
            }
            else if (s[*pos] == 'd') {
                ++(*pos);
                value = decode_dictionary(s, pos);
            }
            // std::cout << key << ": " << value << std::endl;

            str += key + ':' + value + ',';
        }
        str += '}';
        *pos += 1;

        return str;
    }

    std::map<std::string, std::string> decode_file(std::string s) {
        std::map<std::string, std::string> m;
        if (s[0] != 'd') {
            throw std::invalid_argument("Error in file (expected 'd' on index 0)");
        }
        if (s[s.size()-1] != 'e') {
            throw std::invalid_argument("Error in file (expected 'e' on last index)");
        }
        uint32_t pos = 1;


        std::string decoded = decode_dictionary(s, &pos);

        std::cout << decoded << std::endl;

        return m;

    }



}


#endif //BENCODE_H
