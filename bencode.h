#ifndef BENCODE_H
#define BENCODE_H

#include <any>
#include <iostream>
#include <map>
#include <string>
#include <variant>


namespace bencode {
    std::map<std::string, std::string> decode(std::string s) {
        std::map<std::string, std::string> m;
        int pos = 0;
        if (s[pos] != 'd') {
            throw std::invalid_argument("Error in file (expected 'd' on index 0)");
        }
        pos+=2;
        while (s[pos] != 'e') {

            std::string unparsed = s.substr(pos, 2);
        }


        return m;

    }
}


#endif //BENCODE_H
