#include <iostream>
#include "bbqclient.h"
#include "../bbqprotocol/BBQProtocol.h"

int main()
{
    std::cout << "Hello Client World!\n";

    BbqClient client = BbqClient();
    client.initialize("127.0.0.1", 8080);
    
    std::cout << "1. " << BBQCommands::Client::I_AM_HUNGRY << std::endl;
    std::cout << "2. " << BBQCommands::Client::I_TAKE_THAT << std::endl;
    std::cout << "3. " << BBQCommands::Client::NO_THANKS << std::endl;
    std::cout << "4. Quit" << std::endl;

    int cmd = 0;
    std::cin >> cmd;
    while (cmd != 4) {
        switch (cmd) {
        case 1:
            std::cout << client.requestBbq() << std::endl;
            break;
        case 2:
            std::cout << client.acceptFood() << std::endl;
            break;
        case 3:
            std::cout << client.rejectFood() << std::endl;
            break;
        }
        std::cin >> cmd;
    }
    
}

