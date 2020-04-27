#include "BBQProtocol.h"

const std::string BBQCommands::Client::I_AM_HUNGRY = "I AM HUNGRY, GIVE ME BBQ";
const std::string BBQCommands::Client::NO_THANKS = "NO THANKS";
const std::string BBQCommands::Client::I_TAKE_THAT = "I TAKE THAT!!!";

const std::string BBQCommands::Server::OK_WAIT = "OK, WAIT";
const std::string BBQCommands::Server::CLOSED_BYE = "CLOSED BYE";
const std::string BBQCommands::Server::SERVED_BYE = "SERVED BYE";
const std::string BBQCommands::Server::CHICKEN_READY = "CHICKEN READY";
const std::string BBQCommands::Server::BEEF_READY = "BEEF READY";
const std::string BBQCommands::Server::LAST_MONTH_MAMMOTH_READY = "LAST MONTH MAMMOTH READY";