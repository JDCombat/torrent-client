#include <fstream>
#include <iostream>
#include "bencode.h"
#include "cpr/cpr.h"

int main() {
    std::ifstream in;
    in.open("../sample.torrent", std::fstream::binary);
    std::string line;
    
    std::string text;
    auto buffer = in.rdbuf();

    std::ostringstream os;
    os << buffer;
    text = os.str();

    in.close();

    std::cout << "textdsfdfs: " << text[44999] << std::endl;

    auto decoded = bencode::decode_file(text);

    // cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
    //               cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
    //               cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    // r.status_code;                  // 200
    // r.header["content-type"];       // application/json; charset=utf-8
    // std::cout << r.text;                         // JSON text string


    return 0;
}