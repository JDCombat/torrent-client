#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include <string>
#include "cpr/cpr.h"
#include <thread>
#include "cmake-build-debug/_deps/libportfwd-src/include/portfwd/portfwd.h"
#include <sys/socket.h>

struct peer {
    std::string host;
    uint16_t port{};
};

class Downloader {
    public:

    std::string announceURL;
    std::string hash;

    std::vector<peer> peers;

    std::atomic_bool running = false;

    std::string event;

    std::thread requestThread;

    Downloader(const std::string &announceURL, const std::string &hash) {
        if (announceURL.starts_with("udp")) {
            throw std::runtime_error("UDP not supported");
        }
        this->announceURL = announceURL;
        this->hash = hash;
    };
    ~Downloader() = default;
    void start();
    void requestAnnounce();
};

inline void Downloader::start() {

    Portfwd pf;
    if(!pf.init(2000))
    {
        printf("Portfwd.init() fdsfs failed.\n");
        return;
    }
    // printf("External IP: %s\n", pf.external_ip().c_str());
    // printf("LAN IP: %s\n", pf.lan_ip().c_str());
    // printf("Max upstream: %d bps, max downstream: %d bps\n",
    //        pf.max_upstream_bps(), pf.max_downstream_bps() );

    printf("%s\n", ((pf.add( 61420 ))?"Added":"Failed to add") );

    event = "started";
    running = true;
    requestThread = std::thread([this] {requestAnnounce();});
    requestThread.detach();


    while (running) {
        for (auto& peer : peers) {
            std::cout << peer.host << ":" << peer.port << "\n";
            auto socket = new clientSocket(peer.host, peer.port);
            socket->connectSocket();
            std::string message = "\x13";
            socket->sendSocket()
            delete socket;
        }
    }
}
inline void Downloader::requestAnnounce() {
    while (running) {
        cpr::Parameters params;
        if (!event.empty()) {
            params = cpr::Parameters{{"compact", "1"}, {"info_hash", hash}, {"peer_id", "MxVpOJmHKKwgCbMhNNl3"}, {"event", event}, {"port", "61420"}, {"numwant", "200"}};
            event.clear();
        }
        else {
            params = cpr::Parameters{{"compact", "1"}, {"info_hash", hash}, {"peer_id", "MxVpOJmHKKwgCbMhNNl3"}, {"port", "61420"}, {"numwant", "200"}};
        }

        cpr::Response r = cpr::Get(cpr::Url{this->announceURL},
                                params,
                                cpr::Timeout{100000}
                  );
        // r.status_code;                  // 200
        // r.header["content-type"];       // application/json; charset=utf-8
        std::cout << r.url << "\n";
        std::cout << r.text << "\n";                         // JSON text string2

        std::string toDecode = r.text;

        std::string json_string = bencode::decode_dictionary(toDecode, new uint32_t(1));
        std::cout << json_string << "\n";
        auto parsed = nlohmann::json::parse(json_string);
        if (parsed["failure reason"].is_string()) {
            std::cout << "Failure: " << parsed["failure reason"] << "\n";
            return;
        }

        auto peersString = parsed["peers"].get<std::string>();


        for (size_t i = 0; i < peersString.size(); i+=12) {
            peer p;

            std::stringstream ss;
            ss << std::dec << stoi(peersString.substr(i, 2), nullptr, 16);
            ss << "." << std::dec << stoi(peersString.substr(i + 2, 2), nullptr, 16);
            ss << "." << std::dec << stoi(peersString.substr(i + 4, 2), nullptr, 16);
            ss << "." << std::dec << stoi(peersString.substr(i + 6, 2), nullptr, 16);

            p.host = ss.str();
            ss.str(std::string());
            ss << std::dec << stoi(peersString.substr(i + 8, 4), nullptr, 16);
            p.port = atoi(ss.str().c_str());

            std::cout << p.host << ":" << p.port << "\n";
            peers.push_back(p);
        }

        std::this_thread::sleep_for(std::chrono::seconds(parsed["interval"].get<int>()));
    }
}

#endif //DOWNLOADER_H
