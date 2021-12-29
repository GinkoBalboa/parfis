/**
 * @file main.cpp
 *
 * @brief Main file of parfisApp.
 */
/** 
 * @brief Executable terminal interface for parfis lib
 * @defgroup pafisApp
 * @{
 */
#include <iostream>
#include <iomanip>
#include "parfis.h"

using namespace parfis;

/**
 * @brief This string is returned on help() command
 */
std::string helpString = "\n\
help()                - Prints this message.\n\
info()                - Information about the global settings of the program and\n\
                        existing Parfis objects.\n\
info(id)              - Information about the Parfis object with given id.\n\
config(id)            - Prints the configuration for Parfis object with id.\n\
config(id, string)    - Configures Parfis object with id. Configuration string is in the\n\
                        format \"key=value\".\n\
quit()                - Exits this application.\n";

/**
 * @brief This works as a terminal interfacing the parfis lib.
 * @return Zero on success
 */
int main() {

    std::cout << "Running parfisApp terminal version: " << api::version() << std::endl;
    std::cout << "Type \"quit()\" to quit, or \"help()\" for full list of commands." << std::endl;
    std::string userin;
    while(userin != "quit()") {
        std::cout << "> ";
        std::getline(std::cin, userin);
        std::cin.clear();
        std::cin.sync();
        if (userin.substr(0, 5) == "info(" && userin[userin.size()-1] == ')') {
            if (userin == "info()") {
                std::cout << api::info() << std::endl;
            }
            else {
                std::string sstr = userin.substr(5,userin.size()-6);
                uint32_t id = strtol(sstr.c_str(),nullptr,10);
                if (std::to_string(id) != sstr) {
                    std::cout << "Argument is invalid" << std::endl;
                }
                else {
                    std::cout << api::parfisInfo(id) << std::endl;
                }
            }
        }
        else if (userin == "new()") {
            uint32_t id = api::newParfis();
            std::cout << "new Parfis object created with id=" << id << std::endl;
        }
        else if (userin == "help()") {
            std::cout << helpString << std::endl;
        }
        else if (userin.substr(0, 7) == "config(" && userin[userin.size()-1] == ')') {
            
            if (userin == "config()") {
                std::cout << "defaul configuration for Parfis objects:\n\n";
                std::cout << api::defaultConfiguration() << std::endl;
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
                    int retval = api::configure(id, cfgstr.c_str());
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
                    std::cout << api::configuration(id) << std::endl;
                }
            }
        }
    }
    std::cout << "Quiting parfisApp, cheers!" << std::endl;
    return 0;
}

/** @} pafisApp */