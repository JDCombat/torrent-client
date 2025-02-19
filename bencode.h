#ifndef BENCODE_H
#define BENCODE_H

#include <any>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <include/HTTPRequest.hpp>
#include <vector>


namespace bencode {
    inline std::string decode_string(std::string s, int* pos) {
        std::string str_length_str = s.substr(*pos, s.find(':', *pos+1));
        std::cout << "str length " << str_length_str << std::endl;
        *pos += str_length_str.length();
        std::string str = s.substr(*pos, stoi(str_length_str));
        *pos += stoi(str_length_str);
        return str;
    }
    std::map<std::string, std::any> decode_file(std::string s) {
        std::map<std::string, std::any> m;
        if (s[0] != 'd') {
            throw std::invalid_argument("Error in file (expected 'd' on index 0)");
        }
        if (s[s.size()-1] != 'e') {
            throw std::invalid_argument("Error in file (expected 'e' on index 0)");
        }
        int pos = 1;
        while (s[pos] != 'e') {
            if (std::__format::__is_digit(s[pos])) {
                std::string key = decode_string(s, &pos);
                std::cout << "pozycja" << pos << std::endl;
                std::cout << key << std::endl;
                std::string value = decode_string(s, &pos);
                std::cout << value << std::endl;
            }
            // else if (s[pos] == 'i') {
            //
            // }
            // else if (s[pos] == 'l') {
            //
            // }
            // else if (s[pos] == 'd') {
            //
            // }
        }


        return m;

    }



}


#endif //BENCODE_H
