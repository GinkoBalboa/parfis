#include <iostream>
#include "parfis.h"

using namespace parfis;

int main() {

    std::cout << "Running parfisApp terminal version: " << parfisAPI::version() << std::endl;
    std::cout << "Type \"quit()\" to quit, or \"help()\" to see full list of commands." << std::endl;
    std::string userin;
    while(userin != "quit()") {
        std::cout << "> ";
        std::cin >> userin;
    }
    std::cout << "Quiting parfisApp, cheers!" << std::endl;
    return 0;
}