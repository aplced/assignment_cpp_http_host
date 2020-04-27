#ifndef __HOST_H__
#define __HOST_H__

#include <winsock2.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class Server {
private:    
    int server_fd;
    struct sockaddr_in address;
    WSADATA wsaData;
    char* buffer;
public:
    void initialize(const int port);
    void waitForConnection();
    Server(): 
        buffer(nullptr), 
        server_fd(-1){}
    virtual ~Server();
protected:
    virtual void processConnection(int socket) = 0;
    std::string readMessage(int socket);
    void writeMessage(int socket, std::string message);
};

#endif