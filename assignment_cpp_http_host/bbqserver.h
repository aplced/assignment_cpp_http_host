#include "server.h"
#include <string>
#include <iostream>

class BBQServer : public Server {
private:
	enum OrderState {
		NoOrder,
		Requested,
		Mammoth,
		Chicken,
		Beef
	};
	OrderState orderInProcess = OrderState::NoOrder;
	std::string prepareHttpHeader(std::string body);
	std::string prepareCompleteHttpResponse(std::string body);
protected:
	void processConnection(int socket) override;
};