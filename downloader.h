#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include <string>
#include "cpr/cpr.h"
#include <thread>
#include "cmake-build-debug/_deps/portfwd-src/include/portfwd/portfwd.h"
#include <sys/socket.h>

#include "udpSocket.h"
#include "utpTypes.h"

struct peer {
    std::string host;
    uint16_t port{};
    tcpSocket* socket = nullptr;
    int seqNr = 1;
};

class Downloader {
    public:

    std::string announceURL;
    std::string hash;
    std::vector<peer> peers;
    std::atomic_bool running = false;
    std::string event;
    std::thread requestThread;
    int peersNr = 200;
    udpSocket* utpSocket = nullptr;

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
    void run();
};

inline void Downloader::start() {

    Portfwd pf;
    if(!pf.init(2000))
    {
        printf("Portfwd.init() fdsfs failed.\n");
    }
    // printf("External IP: %s\n", pf.external_ip().c_str());
    // printf("LAN IP: %s\n", pf.lan_ip().c_str());
    // printf("Max upstream: %d bps, max downstream: %d bps\n",
    //        pf.max_upstream_bps(), pf.max_downstream_bps() );

    // printf("%s\n", ((pf.add( 61420 ))?"Added":"Failed to add") );

    event = "started";
    running = true;
    requestThread = std::thread([this] {requestAnnounce();});
    requestThread.detach();

    utpSocket = new udpSocket();
    run();
}

void handshakePeer(peer& peer, udpSocket* socket, std::string& hash) {
    // TCP variant - very rare now apparently :(
    // peer.socket = new tcpSocket(peer.host, peer.port);
    // bool connected = peer.socket->connectSocket();
    // std::string message = "\x13";
    // message += "BitTorrent protocol";
    // message += "\x00\x00\x00\x00\x00\x00\x00\x00";
    // message += hash;
    // message += "jakjanienawidzebozga";
    // if (connected) {
    //     peer.socket->sendSocket(message);
    //     peer.socket->recvSocket(68)
    // }

    utpPacket packet{};
    packet.set_type(ST_SYN);
    packet.set_version(1);
    packet.conn_id = htons(43);
    packet.ack_num = htons(0);
    packet.seq_num = htonl(peer.seqNr);
    timeval t{};
    gettimeofday(&t, nullptr);
    packet.timestamp = htonl(t.tv_usec);

    sockaddr_in sendAddr{};
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons(peer.port);
    sendAddr.sin_addr.s_addr = inet_addr(peer.host.c_str());


    socket->sendSocket(packet, sendAddr);
    auto output = socket->recvSocket();


    if (output.first.get_type() == ST_STATE && output.first.get_version() == 1) {
        std::cout << "Peer: " << peer.host << " | SUCCESS: Received ST_STATE. Sending ST_DATA with handshake." << std::endl;

        utpPacket data{};
        data.set_version(1);
        data.set_type(ST_DATA); // Type 0

        data.conn_id = htons(output.first.conn_id);
        data.ack_num = htons(output.first.seq_num);
        peer.seqNr++;
        data.seq_num = htons(peer.seqNr);

        gettimeofday(&t, nullptr);
        uint32_t current_timestamp_micros = (t.tv_sec * 1000000) + t.tv_usec;
        data.timestamp = htonl(current_timestamp_micros);
        data.timestamp_diff = htonl(current_timestamp_micros - output.first.timestamp);

        data.window_size = htonl(output.first.window_size); // Echo the window size

        char message[68];
        memcpy(message, "\x13", 1);
        memcpy(message + 1, "Bittorrent protocol", 19);
        memcpy(message + 20, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
        memcpy(message + 28, hash.c_str(), 20);
        memcpy(message + 48, "jakjanienawidzebozga", 20);
        data.data = (uint8_t*)message;
        data.data_len = 68;

        std::cout << "DEBUG: message array content before sendSocket:\n";
        for (int i = 0; i < 68; ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)message[i] << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl;
        }

        std::cout << "DEBUG: data.type_ver before sendSocket: " << std::hex << (int)data.type_ver << std::dec << std::endl;
        std::cout << std::dec << std::endl;

        socket->sendSocket(data, sendAddr);

        auto response = socket->recvSocket();
        std::cout << "Peer: " << peer.host << " | Received: " << (int)response.first.get_type() << std::endl;
        std::cout << "Data " << response.first.data;
    }
}

inline void Downloader::run(){
    while (peers.size() != peersNr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::vector<std::thread> threads;

    for (auto& peer : peers) {
        threads.emplace_back(handshakePeer, std::ref(peer), utpSocket, std::ref(hash));
    }
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

inline void Downloader::requestAnnounce() {
    while (running) {
        cpr::Parameters params;
        if (!event.empty()) {
            params = cpr::Parameters{{"compact", "1"}, {"info_hash", hash}, {"peer_id", "jakjanienawidzebozga"}, {"event", event}, {"port", "61420"}, {"numwant", std::to_string(peersNr)}};
            event.clear();
        }
        else {
            params = cpr::Parameters{{"compact", "1"}, {"info_hash", hash}, {"peer_id", "jakjanienawidzebozga"}, {"port", "61420"}, {"numwant", std::to_string(peersNr)}};
        }

        cpr::Response r = cpr::Get(cpr::Url{this->announceURL},
                                params,
                                cpr::Timeout{100000}
                  );
        // std::cout << r.url << "\n";
        // std::cout << r.text << "\n";                         // JSON text string2

        std::string toDecode = r.text;

        std::string json_string = bencode::decode_dictionary(toDecode, new uint32_t(1));
        // std::cout << json_string << "\n";
        auto parsed = nlohmann::json::parse(json_string);
        if (parsed.contains("failure reason")) {
            std::cout << "Failure: " << parsed.at("failure reason") << "\n";
            return;
        }

        auto peersString = parsed["peers"].get<std::string>();


        for (size_t i = 0; i + 12 <= peersString.size(); i += 12) {
            peer p;

            std::stringstream ss;
            ss << std::dec << std::stoi(peersString.substr(i, 2), nullptr, 16);
            ss << "." << std::stoi(peersString.substr(i + 2, 2), nullptr, 16);
            ss << "." << std::stoi(peersString.substr(i + 4, 2), nullptr, 16);
            ss << "." << std::stoi(peersString.substr(i + 6, 2), nullptr, 16);

            p.host = ss.str();

            uint16_t port = static_cast<uint16_t>(std::stoi(peersString.substr(i + 8, 4), nullptr, 16));
            p.port = port;

            peers.push_back(p);
        }

        std::this_thread::sleep_for(std::chrono::seconds(parsed["interval"].get<int>()));
    }
}

#endif //DOWNLOADER_H
