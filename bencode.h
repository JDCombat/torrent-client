
#ifndef BENCODE_H
#define BENCODE_H

#include <iostream>
#include <map>
#include <string>

struct types {
    int int_type;
    std::string string_type;
    std::string array_type[];
    std::map<std::string, types> map_type;
};

namespace bencode {
    std::string decode(std::string s) {
        std::map<std::string, types> m;


    }
}


#endif //BENCODE_H
