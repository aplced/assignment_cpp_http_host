#include "bbqclient.h"
#include <ws2tcpip.h>
#include <iostream>

#include "../bbqprotocol/BBQProtocol.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define LINE_BREAK "\r\n"
#define RECV_BUFF_SZ 30000

BbqClient::~BbqClient() {
    if (recvbuf != nullptr) {
        free(recvbuf);
    }
}

std::string BbqClient::prepareHttpHeader(std::string body) {
    return "GET / HTTP/1.1" LINE_BREAK "Content-Type: text / plain" LINE_BREAK "User-Agent : BBQ Client" LINE_BREAK "Accept : */*" LINE_BREAK 
        "Host: " + host + ":" + std::to_string(port) + LINE_BREAK
        "Accept-Encoding: gzip, deflate, br" LINE_BREAK "Connection: keep-alive" LINE_BREAK "Content-Length: " + std::to_string(body.length()) + LINE_BREAK LINE_BREAK;
}

std::string BbqClient::prepareCompleteHttpResponse(std::string body) {
    return prepareHttpHeader(body) + body;
}

void BbqClient::initialize(std::string host, int port) {

    this->host = host;
    this->port = port;
    struct addrinfo* result = NULL;
    struct addrinfo hints;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "wsa startup failed" << std::endl;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    
    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result)) {
        std::cout << "Failed to resolve the server address and port" << std::endl;
    }

    for (struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // Create a SOCKET for connecting to server
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            std::cout << "failed to create socket" << std::endl;
            WSACleanup();
        }

        // Connect to server
        if (connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    recvbuf = (char*)malloc(RECV_BUFF_SZ);
}

void BbqClient::sendCommand(std::string cmd) {
    send(connectSocket, cmd.c_str(), cmd.length(), 0);
}

std::string BbqClient::processResponse() {
    int bytesRead = 0;

    memset(recvbuf, 0, RECV_BUFF_SZ);
        
    while (bytesRead < strlen("HTTP/1.1 200 OK" LINE_BREAK "Content-Type: text/plain" LINE_BREAK "Content-Length: ")) {
        bytesRead += recv(connectSocket, recvbuf + bytesRead, RECV_BUFF_SZ - bytesRead, 0);
    }

    if (bytesRead > 0) {
        std::string response = std::string(recvbuf, bytesRead);

        int contentLenPos = response.find("Content-Length: ");
        if (contentLenPos > -1) {
            contentLenPos += strlen("Content-Length: ");
            int contentLenPosEnd = response.find(LINE_BREAK, contentLenPos);
            
            while (contentLenPosEnd == -1) {
                bytesRead += recv(connectSocket, recvbuf + bytesRead, RECV_BUFF_SZ - bytesRead, 0);
                contentLenPosEnd = response.find(LINE_BREAK, contentLenPos);
            }

            //Assuming complete packet...
            std::string cmdLenString = std::string(recvbuf + contentLenPos, contentLenPosEnd - contentLenPos);
            int bbqCommandSize = std::stoi(cmdLenString);

            int bbqCmdPos = response.find(LINE_BREAK LINE_BREAK);

            if (bbqCmdPos > -1) {
                bbqCmdPos += 2 * strlen(LINE_BREAK);

                if (bytesRead - bbqCmdPos == bbqCommandSize) {
                    return std::string(recvbuf + bbqCmdPos, bbqCommandSize);
                }
                else {
                    return "ERROR: HTTP content size and bbq command don't add up";
                }
            }
            else {
                return "ERROR: No BBQ response";
            }
        }
        else {
            return "ERROR: Can't parse content length";
        }
    }
    else {
        return "Socket recv encountered an error " + std::to_string(WSAGetLastError());
    }
}

std::string BbqClient::requestBbq() {
    static std::string requestFood = prepareCompleteHttpResponse(BBQCommands::Client::I_AM_HUNGRY);
    sendCommand(requestFood);

    return processResponse();
}

std::string BbqClient::acceptFood() {
    static std::string yesPlease = prepareCompleteHttpResponse(BBQCommands::Client::I_TAKE_THAT);
    sendCommand(yesPlease);

    return processResponse();
}

std::string BbqClient::rejectFood() {
    static std::string noThanks = prepareCompleteHttpResponse(BBQCommands::Client::NO_THANKS);
    sendCommand(noThanks);

    return processResponse();
}