#ifndef __HOST_H__
#define __HOST_H__

#include <winsock2.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class Server {
private:    
    SOCKET serverSocket;
    struct sockaddr_in address;
    WSADATA wsaData;
    char* buffer;
public:
    void initialize(const int port);
    void waitForConnection();
    Server(): 
        buffer(nullptr), 
        serverSocket(INVALID_SOCKET){}
    virtual ~Server();
protected:
    virtual bool processConnection(SOCKET socket) = 0;
    std::string readMessage(SOCKET socket);
    void writeMessage(SOCKET socket, std::string message);
};

#endif