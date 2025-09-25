#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

#include "./webserv.hpp"


struct ParserVariables
{
    std::string                         buffer;
    std::vector<std::string>            config_array;
    std::vector<std::string>::iterator  it;
    std::string                         token;
    bool                                in_server;
    bool                                in_location;
    ServerConfig                        cur_server;
    size_t                              cur_server_index;
    LocationConfig                      cur_loc;
    int                                 cur_loc_index;
};


class ConfigParser
{
    private:
        std::vector<ServerConfig>   _servers;
        std::vector<std::string>    cp_split(const std::string& str, char delimiter);
        std::string                 trim(const std::string& str);
        unsigned long               str_to_unsigned_long(const std::string& s);
        void                        parseFile(const std::string& filename, ParserVariables& vars);
        void                        parseConfigFile(ParserVariables& vars);
        void                        handleBracketStack(ParserVariables& vars);
        void                        listenToken(ParserVariables& vars);
        void                        serverNameToken(ParserVariables& vars);
        void                        clientMaxBodySizeToken(ParserVariables& vars);
        void                        errorPageToken(ParserVariables& vars);
        void                        uploadStoreToken(ParserVariables& vars);
        void                        cgiExtensionToken(ParserVariables& vars);
        void                        redirectToken(ParserVariables& vars);
        void                        autoIndexToken(ParserVariables& vars);
        void                        methodsToken(ParserVariables& vars);
        void                        indexToken(ParserVariables& vars);
        void                        rootToken(ParserVariables& vars);
        void                        defaultServerRoot(ParserVariables& vars);
        void                        defaultServerIndex(ParserVariables& vars);
        void                        printParsedConfig(const std::vector<ServerConfig>& servers);
        bool                        isMisconfiguredLocation(ParserVariables& vars);
        bool                        isMisconfiguredServer(ParserVariables& vars);

    public:
        ConfigParser();
        ConfigParser(const std::string& filename);
        ConfigParser(const ConfigParser& src);
        ConfigParser& operator=(const ConfigParser& src);
        ~ConfigParser();

        const std::vector<ServerConfig>& getServers() const;

        class DuplicateVariablesException : public std::exception {
            std::string _msg;
        public:
            DuplicateVariablesException(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Duplicate variable.\033[0m";}
            virtual ~DuplicateVariablesException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class MissingValueException : public std::exception {
            std::string _msg;
        public:
            MissingValueException(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Missing variable value.\033[0m";}
            virtual ~MissingValueException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class MissingPortException : public std::exception {
            std::string _msg;
        public:
            MissingPortException(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Missing port.\033[0m";}
            virtual ~MissingPortException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class UnknownVariableException : public std::exception {
            std::string _msg;
        public:
            UnknownVariableException(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Unknown variable.\033[0m";}
            virtual ~UnknownVariableException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class ExtraVariablesException : public std::exception {
            std::string _msg;
        public:
            ExtraVariablesException(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Extra variables not supported.\033[0m";}
            virtual ~ExtraVariablesException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class UnknownVariableValueException : public std::exception {
            std::string _msg;
        public:
            UnknownVariableValueException(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Unknown variable value.\033[0m";}
            virtual ~UnknownVariableValueException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class MissingClosingBracketException : public std::exception {
            std::string _msg;
        public:
            MissingClosingBracketException(const std::string& context) throw() {
                _msg = "[" + context + " ] <= \033[1;31m Missing closing bracket.\033[0m";}
            virtual ~MissingClosingBracketException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class MissingClosingSemicolonException : public std::exception {
            std::string _msg;
        public:
            MissingClosingSemicolonException(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Missing semicolon.\033[0m";}
            virtual ~MissingClosingSemicolonException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class MisconfigurationException : public std::exception {
            std::string _msg;
        public:
            MisconfigurationException(const std::string& context) throw() {
                _msg = "[" + context + " ] <= \033[1;31m Misconfigured config file.\033[0m";}
            virtual ~MisconfigurationException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class ErrorCodeMisconfiguration : public std::exception {
            std::string _msg;
        public:
            ErrorCodeMisconfiguration(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Misconfigured error code.\033[0m";}
            virtual ~ErrorCodeMisconfiguration() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class MissingErrorCodePage : public std::exception {
            std::string _msg;
        public:
            MissingErrorCodePage(const std::string& token, const std::string& context) throw() {
                _msg = "[" + context + " ] => \"" + token + "\" <= \033[1;31m Missing error_page path/file.\033[0m";}
            virtual ~MissingErrorCodePage() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
        
        class FileOpenErrorException : public std::exception {
            std::string _msg;
        public:
            FileOpenErrorException(const std::string& file_path) throw() {
                _msg = "[server] => \"" + file_path + "\" <= \033[1;31m Failed opening config file.\033[0m";}
            virtual ~FileOpenErrorException() throw() {}
            const char* what() const throw() { return _msg.c_str(); }
        };
};


#endif
