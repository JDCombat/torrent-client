#ifndef BENCODE_H
#define BENCODE_H

#include <any>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <vector>


namespace bencode {

    std::string decode_string(std::string s, uint32_t* pos);
    std::string decode_int(std::string s, uint32_t* pos);
    std::string decode_array(std::string s, uint32_t* pos);
    std::string decode_dictionary(std::string s, uint32_t* pos);

    inline std::string decode_string(std::string s, uint32_t* pos) {
        std::string str_length_str = s.substr(*pos, s.find(':', *pos+1) - *pos);
        *pos += str_length_str.length()+1;
        std::string str = s.substr(*pos, stoi(str_length_str));
        *pos += stoi(str_length_str);
        return str;
    }

    std::string decode_int(std::string s, uint32_t* pos) {
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
    std::string decode_array(std::string s, uint32_t* pos) {
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
                    str += decode_string(s, pos) + ',';
                    break;
            }
        }
        str = str.substr(0, str.length()-1)+"]";
        *pos +=1;
        return str;
    }

    std::string decode_dictionary(std::string s, uint32_t* pos) {
        std::string str = "{";
        while (s[*pos] != 'e') {
            std::string key = decode_string(s, pos);
            std::string value;
            std::cout << "klucz " << key << std::endl;
            std::cout << *pos << std::endl;
            switch (s[*pos]) {
                case 'i':
                    value = decode_int(s, pos);
                    break;
                case 'd':
                    ++(*pos);
                    value = decode_dictionary(s, pos);
                    break;
                case 'l':
                    *pos += 1;
                    value = decode_array(s, pos);
                    break;
                default:
                    value = decode_string(s, pos);
                    break;
            }
            // std::cout << key << ": " << value << std::endl;

            str += key + ':' + value + ",\n";
        }
        *pos += 1;
        str = str.substr(0, str.length()-2)+"}";
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

        std::cout << std::endl << std::endl << std::endl;
        std::cout << decoded << std::endl;

        return m;

    }



}


#endif //BENCODE_H
