#ifndef REQUEST_H
#define REQUEST_H
#include <iostream>
#include <string>

#include "sys/socket.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef (WIN32)
#include "winsock2.h"
#endif

class clientSocket {
    public:

    int sock;
    sockaddr_in addr{};
    std::string hostname;
    unsigned short port;


    clientSocket(const std::string& hostname, unsigned short port, const std::string& protocol = "TCP");
    ~clientSocket();
    void connectSocket();
    void closeSocket();
    void sendSocket(std::string msg);
    std::string recvSocket();
};

inline clientSocket::clientSocket(const std::string& hostname, const unsigned short port, const std::string& protocol): hostname(hostname), port(port) {

    #ifdef WIN32

    #endif

    if (protocol == "TCP") {
        sock = socket(AF_INET, SOCK_STREAM, 0);
    }
    else {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
    }
    if (sock < 0) {
        std::cerr << "Socket creation failed.";
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, hostname.c_str(), &addr.sin_addr.s_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported.";
    }
}


inline void clientSocket::connectSocket() {
    if (connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Connection failed.";
    }
}
inline void clientSocket::closeSocket() {
    if (close(sock) < 0) {
        std::cerr << "Closing socket failed.";
    }
}
inline void clientSocket::sendSocket(std::string msg) {
    send(sock, msg.c_str(), msg.size(), 0);
}
inline std::string clientSocket::recvSocket() {
    char buff[1024] = { 0 };
    recv(sock, buff, 1024, 0);
    return std::string{buff};
}

inline clientSocket::~clientSocket() {
    if (sock >= 0) {
        close(sock);
    }
}





#endif //REQUEST_H
