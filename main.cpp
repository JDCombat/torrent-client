#include <fstream>
#include <bits.h>
#include <iostream>
#include "bencode.h"


int main() {
    std::ifstream in;
    in.open("../sample.torrent", std::ios::binary);
    std::string line;

    std::string text;
    while (std::getline(in, line)) {
        text += line;
        // text.erase(text.length() - 1);
    }
    // text.append("e");
    in.close();
    std::cout << text << std::endl;
    std::cout << text.find(':') << std::endl;
    for (const auto &[k, v] : bencode::decode(text)) {
        std::cout << "m[" << k << "] = (" << std::any_cast<std::string>(v) << ") " << std::endl;
    }
}