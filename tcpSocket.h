#ifndef REQUEST_H
#define REQUEST_H
#include <iostream>
#include <string>

#include "sys/socket.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#if (WIN32)
#include "winsock2.h"
#endif

class tcpSocket {
    public:

    int sock;
    sockaddr_in addr{};
    std::string hostname;
    unsigned short port;


    tcpSocket(const std::string& hostname, unsigned short port);
    ~tcpSocket();
    bool connectSocket();
    void closeSocket();
    void sendSocket(std::string msg);
    std::string recvSocket(size_t expected);
};

inline tcpSocket::tcpSocket(const std::string& hostname, const unsigned short port): hostname(hostname), port(port) {

    #if WIN32

    #endif

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        std::cerr << "Socket creation failed.";
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, hostname.c_str(), &addr.sin_addr.s_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported.";
    }
}


inline bool tcpSocket::connectSocket() {
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    int result = connect(sock, (sockaddr*)&addr, sizeof(addr));
    if (result == 0) {
        fcntl(sock, F_SETFL, flags);
        std::cout << "Connection established." << '\n';
        return true;
    }

    if (errno != EINPROGRESS) {
        std::cerr << "Immediate connection failed: " << std::strerror(errno) << "\n";
        return false;
    }

    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    timeval tv{};
    tv.tv_sec = 15;

    result = select(sock + 1, nullptr, &writefds, nullptr, &tv);
    if (result > 0) {
        int err;
        socklen_t len = sizeof(err);
        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) < 0 || err != 0) {
            std::cerr << "getsockopt failed or error on socket: " << std::strerror(err) << "\n";
            return false;
        }

        fcntl(sock, F_SETFL, flags);
        std::cout << "Connection established." << '\n';
        return true;
    }

    if (result == 0) {
        std::cerr << "Connect timed out\n";
    } else {
        std::cerr << "Select failed: " << std::strerror(errno) << "\n";
    }

    return false;
}
inline void tcpSocket::closeSocket() {
    if (close(sock) < 0) {
        std::cerr << "Closing socket failed.";
    }
}
inline void tcpSocket::sendSocket(std::string msg) {
    std::cout << "wysyłam";
    send(sock, msg.c_str(), msg.size(), 0);
}
std::string tcpSocket::recvSocket(size_t expected = 1024) {
    std::string data;
    char buff[512];

    while (data.size() < expected) {
        ssize_t n = recv(sock, buff, sizeof(buff), 0);
        if (n < 0) {
            std::cerr << "Connection closed. " << std::strerror(errno);
            break;
        }
        data.append(buff, n);
    }
    return data;
}

inline tcpSocket::~tcpSocket() {
    if (sock >= 0) {
        close(sock);
    }
}





#endif //REQUEST_H
