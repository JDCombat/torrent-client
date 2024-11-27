#ifndef BENCODE_H
#define BENCODE_H

#include <any>
#include <iostream>
#include <map>
#include <string>
#include <variant>

// struct types {
//     int int_type;
//     std::string string_type;
//     std::map<std::string, types> map_type;
//     std::array<std::string> array_type;
// };

namespace bencode {
    std::map<std::string, std::any> decode(std::string s) {
        std::map<std::string, std::any> m;
        std::string mode = "dic";
        for (int i = 1; i < 40; i++) {
            m[s.substr(i + 2, s[i])] = std::any(s.find(s[i]));
        }

        return m;

    }
}


#endif //BENCODE_H
