#include <iostream>
#include <iomanip>
#include "parfis.h"

using namespace parfis;

std::string helpString = "\
\nhelp()                - Prints this message.\
\ninfo()                - Information about the global settings of the program and\
\n                        existing Parfis objects.\
\ninfo(id)              - Information about the Parfis object with given id.\
\nconfig(id)            - Prints the configuration for Parfis object with id.\
\nconfig(id, string)    - Configures Parfis object with id. Configuration string is in the\
\n                        format \"key=value\".\
\nquit()                - Exits this application.";

int main() {

    std::cout << "Running parfisApp terminal version: " << ParfisAPI::version() << std::endl;
    std::cout << "Type \"quit()\" to quit, or \"help()\" for full list of commands." << std::endl;
    std::string userin;
    while(userin != "quit()") {
        std::cout << "> ";
        std::getline(std::cin, userin);
        std::cin.clear();
        std::cin.sync();
        if (userin.substr(0, 5) == "info(" && userin[userin.size()-1] == ')') {
            if (userin == "info()") {
                std::cout << ParfisAPI::info() << std::endl;
            }
            else {
                std::string sstr = userin.substr(5,userin.size()-6);
                uint32_t id = strtol(sstr.c_str(),nullptr,10);
                if (std::to_string(id) != sstr) {
                    std::cout << "Argument is invalid" << std::endl;
                }
                else {
                    std::cout << ParfisAPI::parfisInfo(id) << std::endl;
                }
            }
        }
        else if (userin == "new()") {
            uint32_t id = ParfisAPI::newParfis();
            std::cout << "new Parfis object created with id=" << id << std::endl;
        }
        else if (userin == "help()") {
            std::cout << helpString << std::endl;
        }
        else if (userin.substr(0, 7) == "config(" && userin[userin.size()-1] == ')') {
            
            if (userin == "config()") {
                std::cout << "defaul configuration for Parfis objects:\n\n";
                std::cout << ParfisAPI::defaultConfiguration() << std::endl;
            }
            else {
                std::string sstr = userin.substr(7, userin.size()-8);
                std::string idstr;
                if (sstr.find(',') != std::string::npos ) {
                    idstr = sstr.substr(0, sstr.find(','));
                    uint32_t id = strtol(idstr.c_str(),nullptr,10);
                    std::string rstr = sstr.substr(sstr.find(',')+1, sstr.size() - sstr.find(','));
                    size_t firstChar = rstr.find('\"') + 1;
                    std::string cfgstr = rstr.substr(firstChar, rstr.find('\"', firstChar) - firstChar);
                    std::cout << "configure Parfis, id=" << std::to_string(id) << 
                        ", cfg=\"" << cfgstr << '\"'<< std::endl;
                    int retval = ParfisAPI::configure(id, cfgstr.c_str());
                    if (retval != 0) {
                        if (retval == 1) 
                            std::cout << "Parfis object with id=" <<  std::to_string(id) << 
                                " doesn't exist" << std::endl;
                        else if (retval == 2)
                            std::cout << "domain name not recognized" << std::endl;
                    }
                }
                else {
                    idstr = sstr.substr(0, sstr.find(')'));
                    uint32_t id = strtol(idstr.c_str(),nullptr,10);
                    std::cout << "configuration for Parfis object with id=" << std::to_string(id) << ":\n\n";
                    std::cout << ParfisAPI::configuration(id) << std::endl;
                }
            }
        }
    }
    std::cout << "Quiting parfisApp, cheers!" << std::endl;
    return 0;
}