

<div align="center">

# webserv

### 42 Madrid — Network Branch

![42 School](https://img.shields.io/badge/42-Madrid-000000?style=for-the-badge&logo=42&logoColor=white)
![C++98](https://img.shields.io/badge/C++98-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![HTTP/1.1](https://img.shields.io/badge/HTTP-1.1-e11d48?style=for-the-badge)
![epoll](https://img.shields.io/badge/epoll-non--blocking-4a4a4a?style=for-the-badge)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

</div>

---

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Architecture](#architecture)
- [Instructions](#instructions)
- [Configuration](#configuration)
- [Performance](#performance)
- [Resources](#resources)
- [Team](#team)

---

## Description

webserv is a lightweight HTTP/1.1 server written in C++98. It is single-threaded and fully non-blocking, using `epoll()` to manage multiple client connections within a single process. CGI scripts run in a separate child process — the only exception to the single-threaded model.

The project required building everything from scratch: the config parser, the HTTP parser, the response builder, the CGI handler, and the event loop. No external libraries.

---

## Features

![GET](https://img.shields.io/badge/GET-supported-22c55e?style=flat-square)
![POST](https://img.shields.io/badge/POST-supported-22c55e?style=flat-square)
![DELETE](https://img.shields.io/badge/DELETE-supported-22c55e?style=flat-square)
![HEAD](https://img.shields.io/badge/HEAD-supported-22c55e?style=flat-square)
![CGI](https://img.shields.io/badge/CGI-supported-22c55e?style=flat-square)
![Autoindex](https://img.shields.io/badge/Autoindex-supported-22c55e?style=flat-square)
![Cookies](https://img.shields.io/badge/Cookies-supported-22c55e?style=flat-square)
![Uploads](https://img.shields.io/badge/Uploads-supported-22c55e?style=flat-square)

| Feature | Details |
|---|---|
| HTTP methods | GET, POST, DELETE, HEAD |
| Multiple servers | Different IPs and ports in a single config file |
| Static file serving | Index files, default routes, autoindex generation |
| Error pages | Default and custom per server/location |
| CGI execution | Python, Shell, PHP — any interpreter with a valid shebang; extension-based |
| File uploads | Via `upload_store` directive; stored in RAM |
| Cookies | Server sends and recognizes client cookies |
| Redirects | URL redirects and internal location rewrites |
| Chunked transfer | Partial reads handled in `HttpReceive` |
| Non-blocking I/O | Full `epoll` event loop, no `errno` for error handling |

---

## Architecture

The server is split into six components, each with a single responsibility.

```
                +---------------------------+
                |       ConfigParser        |
                |  parses and validates     |
                |  the .conf file           |
                +-------------+-------------+
                              |
                +-------------+-------------+
                |    ServerWrapper / Server  |
                |  wraps each server block   |
                |  ports, roots, methods,    |
                |  error pages, CGI exts     |
                +-------------+-------------+
                              |
                +-------------+-------------+
                |         Connection         |
                |  socket bind + listen      |
                |  epoll event loop          |
                |  non-blocking accept/read/ |
                |  write                     |
                +------+-------------+-------+
                       |             |
          +------------+--+       +--+------------+
          |  HttpReceive  |       |   HttpSend    |
          |  parses HTTP  |       |  builds and   |
          |  requests,    |       |  sends HTTP   |
          |  headers,     |       |  responses    |
          |  body, chunks |       |  200–505      |
          +---------------+       +---------------+

                +---------------------------+
                |          Logger           |
                |  connections, timeouts,   |
                |  requests, responses,     |
                |  signals — color-coded    |
                +---------------------------+
```

**ConfigParser** validates the config file at startup. It throws detailed exceptions for missing semicolons, duplicate directives, invalid status codes, or unclosed brackets.

**Connection** owns the `epoll` loop. It monitors all file descriptors — listening sockets, client connections, and CGI pipes — and dispatches events without blocking.

**HttpReceive** handles partial reads caused by socket buffer limits and reassembles chunked bodies. Headers are stored in a map; the body is read into memory up to 4 GiB. Uploads are stored fully in RAM before writing.

**HttpSend** handles all response logic: static files, CGI output, upload acknowledgements, autoindex HTML generation, and redirects.

**Logger** prints color-coded runtime events to stdout — connections, disconnections, timeouts, requests, responses, and signals.

![Logger output](https://github.com/user-attachments/assets/59c6c259-7ebf-49f8-a87b-046ffa214d2d)

---

## Instructions

**Requirements**

- Linux
- `g++`, `make`

**Compilation**

```bash
git clone https://github.com/T-D-A-H/webserv.git
cd webserv
make
```

**Run**

```bash
./webserv webserv.conf
```

**Cleanup**

| Command | Description |
|---|---|
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binary |
| `make re` | Full recompile |

A static website is included in the repo to test uploads, form submissions, CGI scripts, redirects, and autoindex pages. Point the server at `webserv.conf` and open `http://localhost:<port>` in a browser.

![Static site](https://github.com/user-attachments/assets/2bfcde69-0c0c-4d31-a1aa-d6248e07f661)

---

## Configuration

Config files follow a Nginx-inspired block syntax. Each server block defines one virtual server.

```nginx
server {
    listen       8080;
    server_name  localhost;
    root         ./www;
    index        index.html;

    error_page   404 ./www/404.html;

    location / {
        methods      GET POST;
        autoindex    on;
    }

    location /upload {
        methods      POST DELETE;
        upload_store ./uploads;
    }

    location /cgi-bin {
        methods      GET POST;
        cgi_ext      .py .sh;
    }

    location /old {
        return       301 /new;
    }
}
```

The parser rejects any misconfiguration at startup with a descriptive error message before the server binds any socket.

![Cookies](https://github.com/user-attachments/assets/cb6615e7-226c-40c0-aeb5-421eacf8eebd)

---

## Performance

Tested with `siege` under heavy load on Linux.

| Test | Result |
|---|---|
| 1,000,000 requests on empty pages | 100% response rate |
| Large file uploads | Handled without drops |
| Client timeouts | Properly enforced |
| Keep-alive connections | Correctly managed |

---

## Resources

**HTTP specification**

- [RFC 7230 — HTTP/1.1 Message Syntax](https://datatracker.ietf.org/doc/html/rfc7230)
- [RFC 7231 — HTTP/1.1 Semantics](https://datatracker.ietf.org/doc/html/rfc7231)
- [MDN — HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)

**epoll and non-blocking I/O**

- [`epoll` man page](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [The C10k Problem](http://www.kegel.com/c10k.html)

**CGI**

- [RFC 3875 — CGI/1.1](https://datatracker.ietf.org/doc/html/rfc3875)



## Team

| Login | GitHub |
|---|---|
| ctommasi | [@vikingokvist](https://github.com/vikingokvist) |
| jaimesan | [@Ja1m3st](https://github.com/Ja1m3st) |

---

<div align="center">
42 Madrid
</div>
