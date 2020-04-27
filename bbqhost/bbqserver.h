#include "server.h"
#include <string>

class BBQServer : public Server {
private:
	enum class OrderState {
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
	bool processConnection(SOCKET socket) override;
};