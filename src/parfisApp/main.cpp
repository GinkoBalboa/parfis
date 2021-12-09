#include <iostream>
#include "parfis.h"

using namespace parfis;

int main() {

    std::cout << "Running parfisApp terminal version: " << cAPI::version() << std::endl;
    std::cout << "Type \"quit()\" to quit, or \"help()\" to see full list of commands." << std::endl;
    std::string userin;
    while(userin != "quit()") {
        std::cout << "> ";
        std::cin >> userin;
        if (userin == "info()") {
            std::cout << cAPI::info() << std::endl;
        }
        if (userin == "new()") {
            std::cout << "new Parfis object created" << std::endl;
            std::cout << "address = " << std::hex << (uint64_t)cAPI::newParfis() << std::endl;
        }
    }
    std::cout << "Quiting parfisApp, cheers!" << std::endl;
    return 0;
}