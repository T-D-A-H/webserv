#include "../includes/ErrorResponse.hpp"
#include "../includes/Connection.hpp" 

void        ErrorResponse::send(int fd, Connection& _connection, int error_code, const std::string& status) {

    std::ostringstream buf;
    std::string body;
    std::string response;
    std::string root = _connection.getServer().getErrorRoot(error_code).c_str();
    std::string file = _connection.getServer().getErrorFile(error_code).c_str();
    std::string full_path = root + file;
    int         error = 1;

    if (isDirectory(root.c_str())) { 

    	std::ifstream file(full_path.c_str(), std::ios::in | std::ios::binary);
    	if (file) {
    		buf << file.rdbuf();
    		body = buf.str();
            error = 0;
    	}
    }
    if (error ==  1) {

            buf << "<!DOCTYPE html><html><head><title>" << status << "</title></head>"
            << "<body><h1>" << status << "</h1></body></html>";	
            body = buf.str();
    }
    
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << "\r\n"
    	<< "Content-Type: text/html\r\n"
    	<< "Content-Length: " << body.size() << "\r\n"
    	<< "Connection: close\r\n\r\n"
    	<< body;

    response = oss.str();
    ::send(fd, response.c_str(), response.size(), 0);
    ::close(fd);
}