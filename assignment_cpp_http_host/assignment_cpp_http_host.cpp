#include <iostream>
#include "bbqserver.h"

int main()
{
    std::cout << "Hello World!\n";

    BBQServer bbq = BBQServer();

    bbq.initialize(8080);
    bbq.waitForConnection();

    std::cout << "Bye bye host!\n";
}
