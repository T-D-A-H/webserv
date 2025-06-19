#include "../includes/webserv.hpp"
#include "../includes/ConfigParser.hpp"


int main(void)
{
    try
    {
        ConfigParser parser("webserv.conf");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return (0);
}
