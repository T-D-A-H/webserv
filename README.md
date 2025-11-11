## webserv

`webserv` is a lightweight HTTP/1.1 server written in C++98.  
It’s a single-threaded, non-blocking implementation using `epoll()` to handle multiple clients efficiently within one process.

### Constraints and Goals
- Single-threaded (no concurrency) — except for CGI execution  
- Fully non-blocking I/O through `epoll`  
- Custom configuration file parser (`.conf`)  
- Implemented HTTP methods: **GET**, **POST**, **DELETE**, and **HEAD**  
- Support for:
  - Multiple servers with different IPs and ports  
  - Default routes and index files  
  - Default and custom error pages  
  - CGI script execution  
  - File uploads via `upload_store`  
  - Cookies  
  - Autoindex generation  
  - Redirects (URLs or internal locations)  
- No use of `errno` for error handling  

### Architecture Overview

**ConfigParser**  
Parses and validates the configuration file.  
Detects misconfigurations and throws detailed exceptions for issues such as missing semicolons, duplicate directives, invalid error codes, or missing brackets.

**Servers / ServerWrapper**  
Wraps each configured server and provides access to configuration data like ports, roots, indices, methods, error pages, and CGI extensions.

**Connection**  
Handles socket creation, binding, and listening.  
Uses `epoll()` to monitor file descriptors and manage client connections.  
Implements non-blocking accept, read, and write handling.

**HttpReceive**  
Parses and validates HTTP requests.  
Handles partial reads due to limited socket buffer size and supports chunked transfer encoding.  
Stores headers in a map and reads the body into memory (up to 4 GiB).  
For uploads, files are fully stored in RAM — writing directly to disk wasn’t implemented due to project constraints.

**HttpSend**  
Sends responses based on the parsed request and HTTP method.  
Implements:
- Normal responses (200, 201, 204)
- Redirections (301, 302)
- Client errors (400–415)
- Server errors (500–505)  

Also serves static files, runs CGI scripts, and manages upload and autoindex responses.

**Logger**  
Prints detailed runtime events:
- Server and client connections/disconnections  
- Timeouts  
- Requests and responses  
- Signals received
<img width="1114" height="174" alt="Screenshot from 2025-11-11 17-21-48" src="https://github.com/user-attachments/assets/59c6c259-7ebf-49f8-a87b-046ffa214d2d" />


Color-coded output improves readability during testing.

### CGI Handling
CGI scripts are executed in a separate process.  
The server handles script output, detects execution errors, and prevents infinite loops.  
Scripts can be any interpreter-supported language with a valid shebang, as long as the extension is allowed in the config.

### Cookies
Cookie detection and responses are supported.
The browser handles most cookie logic, while the server can send and recognize client cookies.

<img width="3832" height="1987" alt="Screenshot from 2025-11-11 17-16-23" src="https://github.com/user-attachments/assets/cb6615e7-226c-40c0-aeb5-421eacf8eebd" />

### Static Website and Testing
A static website is included to test:
- File uploads via POST  
- Form submissions returning dynamic responses  
- CGI script execution (including long or infinite loops)  
- Redirects and autoindex pages

1. Clone the repository:
``` bash
git clone https://github.com/T-D-A-H/webserv.git
cd webserv
```

2. Compile the project:
``` bash
make
```

3. Run the server with the default configuration file:
``` bash
./webserv webserv.conf
```

The server correctly serves static content, uploaded files, and dynamically generated responses.

<img width="3832" height="1987" alt="Screenshot from 2025-11-11 17-14-50" src="https://github.com/user-attachments/assets/2bfcde69-0c0c-4d31-a1aa-d6248e07f661" />

### Performance
Tested using `siege` under heavy load:
- 1,000,000 requests on empty pages → **100% response rate**
- Large payloads (file uploads) handled smoothly  

Client timeouts and keep-alive behavior are properly managed.

---

**Built with:** C++98  
**HTTP Version:** 1.1  
**Platform:** Linux
