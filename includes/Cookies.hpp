// #ifndef Cookies_HPP
// # define Cookies_HPP

// #include <map>
// #include <ctime>
// #include <string>
// #include <iomanip>
// class HttpReceive;

// struct Session
// {
// 	std::string     session_id;
//     int             _current_time;
// };

// class Cookies
// {
//     public:
//         static void addSession(const std::string& session_id) {
//             Session s;
//             s.session_id = session_id;
//             s._current_time = std::time(0);
//             _session[session_id] = s;
//         };

//         static bool hasSession(const std::string& id) {
//             return _session.find(id) != _session.end();
//         };

//         static double getSessionDuration(const std::string& session_id) {
//             std::map<std::string, Session>::iterator it = _session.find(session_id);
//             if (it == _session.end())
//                 return -1.0;

//             time_t now = std::time(NULL);
//             double seconds = difftime(now, it->second._current_time);
//             return seconds;
//         };


//         static std::string parseSessionId(const std::string &cookie_header) {
//             std::string key = "session_id=";
//             std::size_t pos = cookie_header.find(key);
//             if (pos == std::string::npos)
//                 return "";
//             pos += key.size();
//             std::size_t end = cookie_header.find(";", pos);
//             if (end == std::string::npos)
//                 end = cookie_header.size();
//             return cookie_header.substr(pos, end - pos);
//         };

//         static std::string generateSessionId() {
//             static bool seeded = false;
//             if (!seeded) {
//                 std::srand(std::time(0));
//                 seeded = true;
//             }
//             std::ostringstream oss;
//             for (int i = 0; i < 16; ++i) {
//                 int r = std::rand() % 256;
//                 oss << std::hex << std::setw(2) << std::setfill('0') << r;
//             }
//             return oss.str();
//         };

//         static std::string		ensureSession(HttpReceive& request, bool& is_new_session) {
        
//             std::string cookie_header = request.getHeader("Cookie");
//             std::string client_session_id = parseSessionId(cookie_header);
        
//             is_new_session = false;
        
//         	if (client_session_id.empty() || !request.hasSession(client_session_id)) {
//         		std::string new_session_id = generateSessionId();
//         		server.addSession(new_session_id);
//         		is_new_session = true;
//         		return new_session_id;
//         	} else {
//         		return client_session_id;
//         	}
//         };

// };

// #endif
