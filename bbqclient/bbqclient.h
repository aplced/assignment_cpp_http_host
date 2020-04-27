#pragma once

#include <WinSock2.h>
#include <string>

class BbqClient
{
private:
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	std::string prepareHttpHeader(std::string body);
	std::string prepareCompleteHttpResponse(std::string body);
	void sendCommand(std::string cmd);
	std::string processResponse();
	std::string host;
	int port;
	char* recvbuf;
public:
	BbqClient() : recvbuf(nullptr) {}
	~BbqClient();
	void initialize(std::string host, int port);
	std::string requestBbq();
	std::string acceptFood();
	std::string rejectFood();
};

