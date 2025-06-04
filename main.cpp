#include <fstream>
#include <iostream>
#include "bencode.h"
#include "cpr/cpr.h"

int main(int argc, char* argv[]) {

    std::string filename;

    if (argc < 2) {
        std::cerr << "Try " << argv[0] << " -h for more information" << "\n";
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "-f") == 0) {
                if (argv[i+1]) {
                    filename = argv[i+1];
                    i++;
                }
                else {
                    std::cerr << "Filename must be provided" << "\n";
                    exit(1);
                }
            }
            else if ( std::strcmp(argv[i], "-h") == 0 ) {
                std::cout << "client usage: torrent-client [options]" << "\n";
                std::cout << "\t-f <filename> - Path to metainfo file to parse" << "\n";
                std::cout << "\t-h - Help" << "\n";
                exit(0);
            }
            else {
                std::cerr << "Unknown option " << argv[i] << "\n";
                exit(1);
            }
    }

     if (!filename.ends_with(".torrent")) {
         std::cerr << "Invalid file type (required .torrent)" << "\n";
         exit(1);
     }

    std::ifstream in;
    in.open(filename, std::fstream::binary);
    if (!in) {
        std::cerr << "Failed to open file " << filename << "\n";
        exit(1);
    }
    auto buffer = in.rdbuf();

    std::string text;
    std::ostringstream os;
    os << buffer;
    text = os.str();

    in.close();

    auto decoded = bencode::decode_file(text);

    // auto test = bencode::decode_dictionary("d8:completei0e10:downloadedi0e10:incompletei0e8:intervali86400e12:min intervali86400e5:peers0:6:peers60:15:warning message45:info hash is not authorized with this trackere", new uint32_t(1));
    // std::cout << test;

    // cpr::Response r = cpr::Get(cpr::Url{"http://tracker.opentrackr.org:1337/announce"},
    //               cpr::Parameters{{"compact", "1"}, {"info_hash", "\xaa\x75\x0e\x24\x38\x91\xf3\x8b\x9a\x5b\x98\x56\x44\xb3\xb7\x26\xa8\x67\x85\x39"}, {"peer_id", "MxVpOJmHKKwgCbMhNNlS"}, {"event", "started"}},
    //               cpr::Timeout{100000}
    //               );
    // // r.status_code;                  // 200
    // // r.header["content-type"];       // application/json; charset=utf-8
    // std::cout << r.url << std::endl;
    // std::cout << r.text;                         // JSON text string


    std::cout << decoded;


    std::cout << "Comment: " << decoded.at("comment") << "\n";
    std::cout << "Created by: " << decoded.at("created by") << "\n";
    std::time_t time = decoded.at("creation date").get<uint32_t>();
    std::cout << "Creation date: " << std::put_time(std::localtime(&time), "%c %Z") << "\n";
    std::cout << "Announce: " << decoded.at("announce") << "\n";

    if (!decoded["announcgfdsgdsfe-list"].is_null()) {
        std::cout << "Announce list: " << decoded["announce-list"] << "\n";
    }

    // if ()

    // try {
    //     std::cout << "Announces: " << decoded.at("announce-list") << "\n";
    //     std::cout << "Creation date: " << decoded.at("chuj") << "\n";
    //
    //
    // } catch (nlohmann::json_abi_v3_12_0::detail::out_of_range&) {
    // }
    

    return 0;
}