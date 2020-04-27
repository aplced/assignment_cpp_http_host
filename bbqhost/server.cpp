#include "server.h"
#include <cstring>


#define BUFFER_SIZE 30000

Server::~Server() {
    if (buffer != nullptr) {
        free(buffer);
    }
}

void Server::initialize(const int port) {

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        //throw new std::exception("WSA startup");
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        //throw new std::exception("Creating socket");
    }

    memset((char *)&address, 0, sizeof(address)); 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = htonl(INADDR_ANY); 
    address.sin_port = htons(port); 
    if (bind(serverSocket,(struct sockaddr *)&address,sizeof(address)) < 0) { 
        //throw new std::exception("Binding to socket");
    }

    buffer = (char*)malloc(BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
}

void Server::waitForConnection() {
    if (listen(serverSocket, 1) < 0) {
        //throw new std::exception("Listening on socket");
    }

    while(true) {
        int addrlen = sizeof(address);
        int new_socket = accept(serverSocket, (struct sockaddr *)&address, &addrlen);
        if (new_socket < 0) {
            //throw new std::exception("Accepting connection");
        }

        while(processConnection(new_socket));
        closesocket(new_socket);
    }
}

std::string Server::readMessage(SOCKET socket) {
    long valread = recv(socket, buffer, BUFFER_SIZE, 0);
    return std::string(buffer, valread);
}

void Server::writeMessage(SOCKET socket, std::string message) {
    send(socket , message.c_str() , message.length(), 0);
}