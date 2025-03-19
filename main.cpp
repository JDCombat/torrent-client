#include <fstream>
#include <bits.h>
#include <iostream>
#include "bencode.h"
#include "include/HTTPRequest.hpp"


int main() {
    std::ifstream in;
    in.open("../sample.torrent", std::ios::binary);
    std::string line;

    std::string text;
    while (std::getline(in, line)) {
        text += line;
    }
    in.close();
    auto decoded = bencode::decode_file(text);

}