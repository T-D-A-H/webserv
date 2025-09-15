/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <ctommasi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 13:19:49 by jaimesan          #+#    #+#             */
/*   Updated: 2025/09/15 12:25:42 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Connection.hpp"

Connection::Connection(ServerWrapper& _server) : _server(_server) {
	
	this->_fd = this->_server.getFD();
}

Connection::~Connection() {}

bool			Connection::setConnection(ServerWrapper& _server, int listening_fd) {

	std::cout << "setConnection: " <<_previus_full_path << std::endl;
	this->_fd = accept(listening_fd, (struct sockaddr*)_server.getSockAddr(), (socklen_t*)_server.getSockAddr());
	if ( this->_fd < 0) {
		
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
	}
	if (!setRequest()) // Recibimos la Request
		return (false);
	if (!saveRequest(getRequest())) // Guardamos la Request
		return (false);
	return (true);
}
bool			Connection::receiveRequest(ssize_t location_index) {
	
    std::string req_path = _headers["Path"]; // Ej: "/index/estilos.css"
	ServerWrapper& server = this->_server;
	LocationConfig _location = server.getLocation(location_index);
    std::string root = _location.root;       // "www/"
    std::string relative_path;

    // Si la ruta es exactamente igual a la location o termina con '/'
    if (req_path == server.getLocationPath(location_index) || req_path == server.getLocationPath(location_index) + "/") {
		if (_location.indices.size() != 0) {
			relative_path = _location.indices[0];
			std::cout << _location.indices[0] << std::endl;
		} else {
			_previus_full_path = "/";
		}
			
    }
	else {
        // Obtener la parte después de "/index"
        relative_path = req_path.substr(server.getLocationPath(location_index).size());
        if (!relative_path.empty() && relative_path[0] == '/') {
            relative_path.erase(0, 1);  // Quita la barra inicial
		}
    }
	
	_full_path = root + relative_path;
	if (_location.indices.size() != 0) {
		_previus_full_path = _headers["Path"];
	}
    // Construye la ruta completa
	std::cout << "FULL: " << _full_path << std::endl;
	std::cout << "PreviusRequest: " << _previus_full_path << std::endl;
	
    // Verificar archivo

	if (isDirectory(root.c_str()) && _headers["Method"] != "POST") {

		std::cout << "IT IS A DIRECTORY" << std::endl;
		bool found_index = false;
		for (size_t i = 0; i < server.getLocationIndexCount(location_index); i++) {

			std::string index_path = root + server.getLocationIndexFile(location_index, i);
			if (_previus_full_path == "test1/upload.html")
				index_path = _previus_full_path;
			if (fileExistsAndReadable(index_path.c_str())) {
				std::cout << "FILE EXISTS AND READABLE" << std::endl;
				found_index = true;
				break ;
			}
		}
		if (!found_index && server.getAutoIndex(location_index) == true)
			return (SendAutoResponse(root), true);	
		else if (!found_index && server.getAutoIndex(location_index) == false)
			return (send403Response(), false);
	}
/*     else if (!fileExistsAndReadable(_full_path.c_str()) || !checkRequest()) {
        send404Response();
        return (false);
    } */
	
	_file.open(_full_path.c_str());
		
/* 	if (!_file || !checkRequest()) 
		return (send404Response(), false); */
    return (true);
}

bool			Connection::setRequest() {
	
	int bytes_received = recv(getFd(), this->_request, sizeof(this->_request) - 1, 0);
	if (bytes_received < 0) {
		std::cerr << "Error in recv()" << std::endl;
		return (false);
	}
	this->_request[bytes_received] = '\0';
	return (true);
}


bool			Connection::checkRequest() {
	
	if (_headers["Host"].empty()) {
		
		send400Response();
	}
	if (_headers["Method"] != "GET" && _headers["Method"] != "POST" && _headers["Method"] != "DELETE" ) {
		
		send405Response();
		return (false);
	}
	if (!isValidHttpVersion(_headers["Version"])) {
		
		std::cerr << "Unsupported HTTP version: " << _headers["Version"] << std::endl;
		send505Response();
		return (false);
	}
	return (true);
}

bool			Connection::saveRequest(char *request) {
	std::cout << "REQUEST" << std::endl;
	std::cout << "\033[32m" << request << "\033[0m" << std::endl;

	std::string full_request(request);
	std::size_t header_end = full_request.find("\r\n\r\n");
	if (header_end == std::string::npos) {
		send400Response();
		return (false);
	}

	std::string headers_part = full_request.substr(0, header_end);
	std::string body_part = full_request.substr(header_end + 4);
	std::istringstream iss(headers_part);
	std::string line;
	std::getline(iss, line);
	std::istringstream request_line(line);
	std::string method, path, version;
	
	if (!(request_line >> method >> path >> version)) {
		send400Response();
		return (false);
	}


	std::string req_path = path;
	ssize_t best_match = getBestMatch(_server, req_path);
	setBestMatch(best_match);
	
	_headers["Method"] = method;
	_headers["Path"] = path;
	_headers["Version"] = version;

	std::cout << "saveRequest: " <<_previus_full_path << std::endl;
	
	if (method == "POST") {

		// Get de Boundary
		std::size_t content_type = full_request.find("boundary=");
		if (content_type == std::string::npos) {
        	std::cerr << "Error: No se encontró boundary" << std::endl;
    	}
		content_type += 13;
		std::size_t content_length = full_request.find("Content-Length:");
		std::string boundary = full_request.substr(content_type, content_length - content_type - 14);

		// Get the name of file
		std::size_t file_name_init = full_request.find("filename=\"");
		file_name_init += 10;
		std::size_t file_name_end = full_request.find("\"", file_name_init);
		std::string filename = full_request.substr(file_name_init, file_name_end - file_name_init);

		// GET the content
		std::size_t find_init_content = full_request.find("\n", file_name_end + 3);
		find_init_content += 3;
		std::size_t content_boundary = full_request.find(boundary, find_init_content) - 9;


		std::string content = full_request.substr(find_init_content,  content_boundary - find_init_content );

		ServerWrapper& server = this->_server;
		std::string root = server.getLocationRoot(best_match);

		std::string full_path = root + filename;

		std::ofstream file(full_path.c_str());
   		if (!file) {
			std::cerr << "No se pudo crear el archivo en: " << full_path << std::endl;
        	return 1;
   		}
		file << content;
		file.close();
	}
	else {
		
		while (std::getline(iss, line) && !line.empty()) {
			if (!line.empty() && line[line.length() - 1] == '\r') {
				line.erase(line.length() - 1);
			}
			size_t colon = line.find(":");
			if (colon != std::string::npos) {
				std::string key = line.substr(0, colon);
				std::string value = line.substr(colon + 1);
				value.erase(0, value.find_first_not_of(" \t"));
				_headers[key] = value;
			}
		}	
	}
	return (true);
}


void			Connection::sendGetResponse() {
	
	std::ostringstream body_stream;
	body_stream << getFile().rdbuf();
	std::string body = body_stream.str();
	
	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Content-Type: " << getContentType(_full_path) << "\r\n";
	oss << "Content-Length: " << body.size() << "\r\n";
	oss << "Connection: close\r\n\r\n";
	oss << body;

	std::string response = oss.str();
	send(getFd(), response.c_str(), response.size(), 0);
	close(getFd());
}

void			Connection::sendPostResponse() {
	
	// Procesar datos del formulario si es necesario
	std::ostringstream body_stream;
	body_stream << getFile().rdbuf();
	std::string body = body_stream.str();
	std::cout << "PreviusRequest2: " << _previus_full_path << std::endl;
	
	// Redirigir al cliente a index.html
	std::ostringstream oss;
	oss << "HTTP/1.1 303 See Other\r\n";
	oss << "Location: "<< _previus_full_path  << "\r\n";
	oss << "Content-Length: " << body.size() << "\r\n";
	oss << "Connection: close\r\n\r\n";

	std::string response = oss.str();
	send(getFd(), response.c_str(), response.size(), 0);
	close(getFd());
}


void			Connection::SendAutoResponse(const std::string &direction_path) {

	DIR * dir = opendir(direction_path.c_str());
	if (!dir) {
		send403Response();
		return ;
	}
	std::ostringstream body;
    body << "<html><head><title>Index of " << _headers["Path"] << "</title></head><body>";
    body << "<h1>Index of " << _headers["Path"] << "</h1><ul>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        // Skip "." and ".."
        if (name == "." || name == "..")
            continue;

        std::string href = _headers["Path"];
        if (href.empty() || href[href.size()-1] != '/')
            href += "/";
        href += name;

        // Check if directory to add trailing slash
        struct stat st;
        std::string fullPath = direction_path + "/" + name;
        if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            name += "/";
            href += "/";
        }

        body << "<li><a href=\"" << href << "\">" << name << "</a></li>\n";
    }

    body << "</ul></body></html>";
    closedir(dir);

    std::string bodyStr = body.str();
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << bodyStr.size() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << bodyStr;

    send(getFd(), response.str().c_str(), response.str().size(), 0);
}



bool	Connection::isMethodAllowed(Connection& connection, const std::string& method) {
	
	const std::vector<LocationConfig>& locations = connection.getServer().getLocations();
	std::string path = connection.getHeader("Path");
	
	for (size_t j = 0; j < locations.size(); ++j) {
		
		if (path.find(locations[j].path) == 0) {
			
			if (locations[j].methods.empty())
				return (true);
			if (locations[j].methods.count(method))
				return (true);
			else
				return (false);
		}
	}
	return (false);
}

ssize_t			Connection::getBestMatch(ServerWrapper& server, std::string req_path) {
	
	size_t max_match_len = 0;
	ssize_t best_match = -1;
	
	for (size_t j = 0; j < server.getLocations().size(); ++j) {	
		
		const std::string& loc_path = server.getLocationPath(j);
		if (req_path.rfind(loc_path, 0) == 0 && loc_path.size() > max_match_len) {

			max_match_len = loc_path.size();
			best_match = j;
		}
	}
	return (best_match);
}

ssize_t			Connection::getBestMatch() {return (_best_match);}

int				Connection::getFd() {return (this->_fd);}

char*			Connection::getRequest() {return (this->_request);}

std::string		Connection::getHeader(std::string index) {return (this->_headers[index]);}

std::ifstream&	Connection::getFile() {return (this->_file);}

std::string		Connection::getFullPath() {return (this->_full_path);}

ServerWrapper&	Connection::getServer() {return (this->_server);}

void			Connection::setBestMatch(ssize_t _best_match) {this->_best_match = _best_match;}

void			Connection::setFd(int fd) {this->_fd = fd;}

void			Connection::setHeader(std::string index,std::string value) {this->_headers[index] = value;}

void			Connection::setFullPath(const std::string& full_path) {this->_full_path = full_path;}

void			Connection::send200Response() { ErrorResponse::send200(getFd(), *this); }

void			Connection::send201Response() { ErrorResponse::send201(getFd(), *this); }

void			Connection::send204Response() { ErrorResponse::send204(getFd(), *this); }

void			Connection::send301Response() { ErrorResponse::send301(getFd(), *this); }

void			Connection::send302Response() { ErrorResponse::send302(getFd(), *this); }

void			Connection::send400Response() { ErrorResponse::send400(getFd(), *this); }

void			Connection::send401Response() { ErrorResponse::send401(getFd(), *this); }

void			Connection::send403Response() { ErrorResponse::send403(getFd(), *this); }

void			Connection::send404Response() { ErrorResponse::send404(getFd(), *this); }

void			Connection::send405Response() { ErrorResponse::send405(getFd(), *this); }

void			Connection::send413Response() { ErrorResponse::send413(getFd(), *this); }

void			Connection::send414Response() { ErrorResponse::send414(getFd(), *this); }

void			Connection::send500Response() { ErrorResponse::send500(getFd(), *this); }

void			Connection::send501Response() { ErrorResponse::send501(getFd(), *this); }

void			Connection::send502Response() { ErrorResponse::send502(getFd(), *this); }

void			Connection::send503Response() { ErrorResponse::send503(getFd(), *this); }

void			Connection::send504Response() { ErrorResponse::send504(getFd(), *this); }

void			Connection::send505Response() { ErrorResponse::send505(getFd(), *this); }


/* void Connection::sendDeleteResponse() {
	std::ostringstream body_stream;
	body_stream << getFile().rdbuf();
	std::string body = body_stream.str();

	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Content-Type: " << getContentType(getHeader("Path")) << "\r\n";
	oss << "Content-Length: " << body.size() << "\r\n";
	oss << "Connection: close\r\n\r\n";
	oss << body;

	std::string response = oss.str();
	send(getFd(), response.c_str(), response.size(), 0);
	close(getFd());
} */