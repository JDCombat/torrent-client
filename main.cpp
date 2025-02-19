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
    // for (const auto &[k, v] : bencode::decode_file(text)) {
    //     std::cout << "m[" << k << "] = (" << std::any_cast<std::string>(v) << ") " << std::endl;
    // }
    auto decoded = bencode::decode_file(text);
}