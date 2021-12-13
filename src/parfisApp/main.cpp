#include <iostream>
#include "parfis.h"

using namespace parfis;

std::string helpString = 
"\nhelp()       - This message.\
\ninfo()       - Information about the global settings of the program and\
\n               existing Parfis objects.\
\ninfo(<id>)   - Information about the Parfis object with id <id>.\
\nquit()       - Exits this application.";

int main() {

    std::cout << "Running parfisApp terminal version: " << cAPI::version() << std::endl;
    std::cout << "Type \"quit()\" to quit, or \"help()\" to see full list of commands." << std::endl;
    std::string userin;
    while(userin != "quit()") {
        std::cout << "> ";
        std::getline(std::cin, userin);
        std::cin.clear();
        std::cin.sync();
        if (userin.substr(0, 5) == "info(" && userin[userin.size()-1] == ')') {
            if (userin == "info()") {
                std::cout << cAPI::info() << std::endl;
            }
            else {
                std::string sstr = userin.substr(5,userin.size()-6);
                uint32_t id = strtol(sstr.c_str(),nullptr,10);
                if (std::to_string(id) != sstr) {
                    std::cout << "Argument is invalid" << std::endl;
                }
                else {
                    std::cout << cAPI::parfisInfo(id) << std::endl;
                }
            }
        }
        else if (userin == "new()") {
            uint32_t id = cAPI::newParfis();
            std::cout << "new Parfis object created with id = " << id << std::endl;
        }
        else if (userin == "help()") {
            std::cout << helpString << std::endl;
        }
    }
    std::cout << "Quiting parfisApp, cheers!" << std::endl;
    return 0;
}