#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H
#include "sys/socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <bits/ostream.tcc>
#include <netinet/in.h>

class ServerSocket {
    public:
    int serverSocket;
    sockaddr_in serverAddress{};

    unsigned short serverPort;

    explicit ServerSocket(unsigned short port);
    ~ServerSocket();
    void bindSocket(int port);
    void listenSocket();
    int acceptSocket();
    void closeSocket();
};

inline ServerSocket::ServerSocket(unsigned short port): serverPort(port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
}

inline void ServerSocket::bindSocket() {
    if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) > 0) {
        std::cerr << "Server socket bind failed";
    }
}


inline void ServerSocket::listenSocket() {
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Server socket listen failed";
    }
}






#endif //SERVERSOCKET_H
