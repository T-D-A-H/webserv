#include "../includes/webserv.hpp"
#include "../includes/ConfigParser.hpp"


int main() {

    try {
        ConfigParser parser("webserv.conf");
        parser.printParsedConfig(parser.getServers());

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
