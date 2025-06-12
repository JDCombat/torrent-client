#include <fstream>
#include <iostream>
#include "bencode.h"
#include "tcpSocket.h"
#include "downloader.h"
#include "cpr/cpr.h"
#include "openssl/sha.h"


struct fileEntry {
    uint64_t length;
    std::vector<std::string> path;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(fileEntry, length, path);
};


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


    // std::cout << "Comment: " << decoded.at("comment") << "\n";
    std::cout << "Created by: " << decoded.at("created by") << "\n";
    std::time_t time = decoded.at("creation date").get<uint64_t>();
    std::cout << "Creation date: " << std::put_time(std::localtime(&time), "%c %Z") << "\n";
    std::cout << "Announce: " << decoded.at("announce") << "\n";
    std::cout << "Piece length: " << decoded.at("info").at("piece length") << "B\n";

    if (decoded.contains("announce-list")) {
        std::cout << "Announce list: ";
        auto announces = decoded["announce-list"].get<std::vector<std::array<std::string, 1>>>();
        for (auto announce : announces) {
            std::cout << announce[0] << "\t";
        }
        std::cout << "\n";
    }

    if (!decoded.at("info").contains("files")) {
        std::cout << "File " << decoded.at("info").at("name") << "\t Size: " << decoded.at("info").at("length").get<uint64_t>() / 1000 << "KiB\n";
    }
    else {
        std::cout << "File list:\n";
        std::vector<fileEntry> files = decoded.at("info")["files"].get<std::vector<fileEntry>>();
        for (const auto& entry : files) {
            std::cout << decoded.at("info").at("name") << "/";
            for (int i = 0; i < entry.path.size(); i++) {
                std::cout << entry.path[i];
                if (i != entry.path.size() - 1) {
                    std::cout << "/";
                }
            }
            std::cout << "\t" << entry.length << "B\n";
        }
    }

    std::cout << "Do you want to continue?" << "\n";
    char choice;
    std::cin >> std::noskipws >> choice;
    if (choice == 'y') {
        std::string infoDictionary = bencode::encode_dictionary(decoded.at("info"));

        unsigned char hash[SHA_DIGEST_LENGTH];

        SHA1(reinterpret_cast<const unsigned char *>(infoDictionary.c_str()), infoDictionary.size(), hash);

        std::stringstream ss;
        for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        const std::string hashString = ss.str();

        std::string bytesHex;
        for (int i = 0; i < hashString.length() - 1; i+=2) {
            bytesHex += static_cast<char>(std::stoi(hashString.substr(i, 2), nullptr, 16));
        }



        Downloader downloader("http://tracker.opentrackr.org:1337/announce", bytesHex);
        downloader.start();

        // auto socket = clientSocket("192.168.33.4", 20311);
        // bool connected = socket.connectSocket();
        // std::string message = "\x13";
        // message += "BitTorrent protocol";
        // message += "\x00\x00\x00\x00\x00\x00\x00\x00";
        // message += bytesHex;
        // message += "jakjanienawidzebozga";
        // if (connected) {
        //     socket.sendSocket(message);
        //     // std::cout << socket.recvSocket(68);
        // }
    }
    else {
        return 0;
    }

    

    return 0;
}