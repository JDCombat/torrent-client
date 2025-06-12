#ifndef udpsocket_h
#define udpsocket_h
#include <cstring>
#include <iostream>
#include <string>
#include "sys/socket.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>

#include "utpTypes.h"

#if (WIN32)
#include "winsock2.h"
#endif

class udpSocket {
    public:

    int sock;
    sockaddr_in addr{};

    udpSocket();
    ~udpSocket();
    void closeSocket();
    void sendSocket(utpPacket packet, sockaddr_in addr);
    std::pair<utpPacket, sockaddr_in> recvSocket();
};


inline udpSocket::udpSocket() {

    #if WIN32

    #endif

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        std::cerr << "Socket creation failed.\n";
    }

    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(0);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    if ( bind(sock, (sockaddr*)&localAddr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind." << std::strerror(errno) << '\n';
    }
}


inline void udpSocket::closeSocket() {
    if (close(sock) < 0) {
        std::cerr << "Closing socket failed.\n";
    }
}
inline void udpSocket::sendSocket(utpPacket packet, sockaddr_in addr) {
    uint8_t buffer[2048];
    constexpr size_t UTP_HEADER_SIZE = 20;

    // Ensure the buffer is large enough for the header + data
    if (UTP_HEADER_SIZE + packet.data_len > sizeof(buffer)) {
        std::cerr << "Error: Packet too large for buffer. Data truncated or buffer overflow possible.\n";
        return;
    }


    buffer[0] = packet.type_ver;

    buffer[1] = 0;

    std::cout << "DEBUG sendSocket (After header set): buffer[0]=" << std::hex << (int)buffer[0]
          << ", buffer[1]=" << (int)buffer[1] << std::dec << std::endl;

    uint16_t net_conn_id = htons(packet.conn_id);
    memcpy(buffer + 2, &net_conn_id, sizeof(net_conn_id));

    uint32_t net_timestamp = htonl(packet.timestamp);
    memcpy(buffer + 4, &net_timestamp, sizeof(net_timestamp));

    uint32_t net_timestamp_diff = htonl(packet.timestamp_diff);
    memcpy(buffer + 8, &net_timestamp_diff, sizeof(net_timestamp_diff));

    uint32_t net_window_size = htonl(packet.window_size);
    memcpy(buffer + 12, &net_window_size, sizeof(net_window_size));

    uint16_t net_seq_num = htons(packet.seq_num);
    memcpy(buffer + 16, &net_seq_num, sizeof(net_seq_num));

    uint16_t net_ack_num = htons(packet.ack_num);
    memcpy(buffer + 18, &net_ack_num, sizeof(net_ack_num));

    std::cout << "DEBUG sendSocket (After all header serialization): buffer[0]=" << std::hex << (int)buffer[0]
          << ", buffer[1]=" << (int)buffer[1] << std::dec << std::endl;

    if (packet.data != nullptr && packet.data_len > 0) {
        memcpy(buffer + UTP_HEADER_SIZE, packet.data, packet.data_len);
    }

    size_t total_len = UTP_HEADER_SIZE + packet.data_len;
    std::cout << "total length" << total_len << std::endl;

    // DEBUG POINT 3: Check header bytes IMMEDIATELY BEFORE sendto
    std::cout << "DEBUG sendSocket (IMMEDIATELY BEFORE sendto): buffer[0]=" << std::hex << (int)buffer[0]
              << ", buffer[1]=" << (int)buffer[1] << std::dec << std::endl;

    std::cout << "DEBUG sendSocket (Full buffer before sendto):\n";
    for (size_t i = 0; i < total_len; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::dec << std::endl;


    std::cout << addr.sin_addr.s_addr << std::endl;
    std::cout << addr.sin_port << std::endl;

    ssize_t bytes_sent = sendto(sock, &buffer, total_len, 0, (sockaddr*)&addr, sizeof(addr));

    if (bytes_sent < 0) {
        std::cerr << "sendto failed: " << std::strerror(errno) << "\n";
    } else if (bytes_sent != total_len) {
        std::cerr << "Warning: Sent " << bytes_sent << " bytes, but expected " << total_len << " bytes.\n";
    } else {
        std::cout << "Successfully sent " << bytes_sent << " bytes.\n";
    }
}
std::pair<utpPacket, sockaddr_in> udpSocket::recvSocket() {
    char buffer[1024];

    utpPacket pkt{};

    sockaddr_in from{};
    socklen_t len = sizeof(from);
    ssize_t recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&from, &len);

    if (recv_len < 0) {
        std::cerr << "recv failed: " << strerror(errno) << "\n";
        return {};
    }

    constexpr size_t UTP_HEADER_SIZE = 20;

    int dataPos = 0;
    const uint8_t* ptr = reinterpret_cast<uint8_t*>(buffer);

    pkt.type_ver = ptr[0];

    uint16_t temp_conn_id;
    memcpy(&temp_conn_id, ptr + 2, sizeof(temp_conn_id));
    pkt.conn_id = ntohs(temp_conn_id);

    uint32_t temp_timestamp;
    memcpy(&temp_timestamp, ptr + 4, sizeof(temp_timestamp));
    pkt.timestamp = ntohl(temp_timestamp);

    uint32_t temp_timestamp_diff;
    memcpy(&temp_timestamp_diff, ptr + 8, sizeof(temp_timestamp_diff));
    pkt.timestamp_diff = ntohl(temp_timestamp_diff);

    uint32_t temp_window_size;
    memcpy(&temp_window_size, ptr + 12, sizeof(temp_window_size));
    pkt.window_size = ntohl(temp_window_size);

    uint16_t temp_seq_num;
    memcpy(&temp_seq_num, ptr + 16, sizeof(temp_seq_num));
    pkt.seq_num = ntohs(temp_seq_num);

    uint16_t temp_ack_num;
    memcpy(&temp_ack_num, ptr + 18, sizeof(temp_ack_num));
    pkt.ack_num = ntohs(temp_ack_num);

    pkt.data_len = recv_len - UTP_HEADER_SIZE;
    pkt.data = nullptr;
    if (pkt.data_len > 0) {
        pkt.data = new uint8_t[pkt.data_len];
        memcpy(pkt.data, buffer + UTP_HEADER_SIZE, pkt.data_len);
    }

    return {pkt, from};
}

inline udpSocket::~udpSocket() {
    if (sock >= 0) {
        close(sock);
    }
}


#endif //udpsocket_h
