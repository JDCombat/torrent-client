#include <fstream>
#include "bencode.hpp"
#include <bits.h>

int main() {
    std::ifstream in;
    in.open("../sample.torrent", std::ios::binary);
    std::string line;

    std::string text;
    while (std::getline(in, line)) {
        text += line;
        text.erase(text.length() - 1);
    }
    text.append("e");
    in.close();
    std::cout << text << std::endl;
    auto data = bencode::decode(text);
    auto elem = data["info"];
}