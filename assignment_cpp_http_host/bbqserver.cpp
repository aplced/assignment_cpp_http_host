#include "bbqserver.h"
#include "BBQProtocol.h"
#include <thread>
#include <chrono>

#define LINE_BREAK "\r\n"

std::string BBQServer::prepareHttpHeader(std::string body) {
    return "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: " + std::to_string(body.length()) + "\n\n";
}

std::string BBQServer::prepareCompleteHttpResponse(std::string body) {
    return prepareHttpHeader(body) + body;
}

void BBQServer::processConnection(int socket) {
    std::string received = readMessage(socket);

    std::string bbqCommand = "[no http body]";
    int startOfBody = received.find(LINE_BREAK LINE_BREAK);
    if (startOfBody > -1) {
        startOfBody += strlen(LINE_BREAK LINE_BREAK);
        int bbqCommandLen = received.find(LINE_BREAK, startOfBody) - startOfBody;
        bbqCommand = received.substr(startOfBody, bbqCommandLen);
    }

    std::cout << "BBQ command: " << bbqCommand << std::endl;

    if (BBQCommands::Client::I_AM_HUNGRY == bbqCommand) {
        
        switch (orderInProcess)
        {
        case OrderState::NoOrder:
            orderInProcess = OrderState::Requested;
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::OK_WAIT));
            std::cout << BBQCommands::Server::OK_WAIT << std::endl;
            break;
        case OrderState::Requested:
            orderInProcess = OrderState::Mammoth;
            //mammoth is always ready, its last week...
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::LAST_MONTH_MAMMOTH_READY));
            std::cout << BBQCommands::Server::LAST_MONTH_MAMMOTH_READY << std::endl;
            break;
        case OrderState::Mammoth: //The client is constantly asking for food, despite offerings
        case OrderState::Chicken:
        case OrderState::Beef:
        default:
            orderInProcess = OrderState::NoOrder;
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CLOSED_BYE));
            std::cout << BBQCommands::Server::CLOSED_BYE << std::endl;
            break;
        }
    }
    else if (BBQCommands::Client::NO_THANKS == bbqCommand) {
        switch (orderInProcess)
        {
        case OrderState::Mammoth:
            orderInProcess = OrderState::Chicken;
            //Should be busy waiting to actually use cpu to cook
            std::this_thread::sleep_for(std::chrono::milliseconds(2500));
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CHICKEN_READY));
            std::cout << BBQCommands::Server::CHICKEN_READY << std::endl;
            break;
        case OrderState::Chicken:
            orderInProcess = OrderState::Beef;
            //Should be busy waiting to actually use cpu to cook
            std::this_thread::sleep_for(std::chrono::milliseconds(5500));
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::BEEF_READY));
            std::cout << BBQCommands::Server::BEEF_READY << std::endl;
            break;
        case OrderState::Beef: // Well there's nothing else to offer...
        case OrderState::NoOrder:
        case OrderState::Requested:
        default:
            orderInProcess = OrderState::NoOrder;
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CLOSED_BYE));
            std::cout << BBQCommands::Server::CLOSED_BYE << std::endl;
            break;
        }

    }
    else if (BBQCommands::Client::I_TAKE_THAT == bbqCommand) {
        switch (orderInProcess)
        {
        case OrderState::Mammoth:
        case OrderState::Chicken:
        case OrderState::Beef:
            orderInProcess = OrderState::NoOrder;
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::SERVED_BYE));
            std::cout << BBQCommands::Server::SERVED_BYE << std::endl;
            break;
        case OrderState::Requested: //The client taking an order that hasn't been offered yet...
        case OrderState::NoOrder:
        default:
            orderInProcess = OrderState::NoOrder;
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CLOSED_BYE));
            std::cout << BBQCommands::Server::CLOSED_BYE << std::endl;
            break;
        }
    }
    else { // Unrecognized command from the client, probably he is hitting the wrong venue
        writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CLOSED_BYE));
        std::cout << "unrecognized command" << std::endl;
    }
}