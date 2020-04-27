#include "bbqserver.h"
#include "../bbqprotocol/BBQProtocol.h"
#include <thread>
#include <chrono>
#include <iostream>

#define LINE_BREAK "\r\n"

std::string BBQServer::prepareHttpHeader(std::string body) {
    return "HTTP/1.1 200 OK" LINE_BREAK "Content-Type: text/plain" LINE_BREAK "Content-Length: " + std::to_string(body.length()) + LINE_BREAK LINE_BREAK;
}

std::string BBQServer::prepareCompleteHttpResponse(std::string body) {
    return prepareHttpHeader(body) + body;
}

bool BBQServer::processConnection(SOCKET socket) {
    std::string received = readMessage(socket);

    std::cout << received << std::endl << std::endl;

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
            return true;
        case OrderState::Requested:
            orderInProcess = OrderState::Mammoth;
            //mammoth is always ready, its last week...
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::LAST_MONTH_MAMMOTH_READY));
            std::cout << BBQCommands::Server::LAST_MONTH_MAMMOTH_READY << std::endl;
            return true;
        case OrderState::Mammoth: //The client is constantly asking for food, despite offerings
        case OrderState::Chicken:
        case OrderState::Beef:
        default:
            orderInProcess = OrderState::NoOrder;
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CLOSED_BYE));
            std::cout << BBQCommands::Server::CLOSED_BYE << std::endl;
            return false;
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
            return true;
        case OrderState::Chicken:
            orderInProcess = OrderState::Beef;
            //Should be busy waiting to actually use cpu to cook
            std::this_thread::sleep_for(std::chrono::milliseconds(5500));
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::BEEF_READY));
            std::cout << BBQCommands::Server::BEEF_READY << std::endl;
            return true;
        case OrderState::Beef: // Well there's nothing else to offer...
        case OrderState::NoOrder:
        case OrderState::Requested:
        default:
            orderInProcess = OrderState::NoOrder;
            writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CLOSED_BYE));
            std::cout << BBQCommands::Server::CLOSED_BYE << std::endl;
            return false;
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

        return false;
    }
    else { // Unrecognized command from the client, probably he is hitting the wrong venue
        writeMessage(socket, prepareCompleteHttpResponse(BBQCommands::Server::CLOSED_BYE));
        std::cout << "unrecognized command" << std::endl;
        return false;
    }
}