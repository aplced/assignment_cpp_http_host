#pragma once

#include <string>

struct BBQCommands {
	struct Client {
		static const std::string I_AM_HUNGRY;
		static const std::string NO_THANKS;
		static const std::string I_TAKE_THAT;
	};

	struct Server {
		static const std::string OK_WAIT;
		static const std::string CLOSED_BYE;
		static const std::string SERVED_BYE;
		static const std::string CHICKEN_READY;
		static const std::string BEEF_READY;
		static const std::string LAST_MONTH_MAMMOTH_READY;
	};
};