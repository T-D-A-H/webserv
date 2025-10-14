#ifndef SERVER_WRAPPER_HPP
# define SERVER_WRAPPER_HPP


#include "./webserv.hpp"
#include "./ConfigParser.hpp"
#include <ctime>

class ServerWrapper
{
    private:
        const ServerConfig*                 config;
        std::vector<LocationConfig>         _locations;


    public:

        ServerWrapper();
        ServerWrapper(const ServerConfig& cfg);
        ServerWrapper(const ServerWrapper& src);
        ServerWrapper& operator=(const ServerWrapper& src);
        ~ServerWrapper();

        std::string                         getIps(size_t ip_index) const;
        size_t                              getIpCount() const;
        size_t                              getCountIpPorts();
        uint16_t                            getPorts(size_t port_index) const;
        size_t                              getPortCount() const;
        std::string                         getServerName(size_t server_name_index) const;
        std::vector<std::string>            getServerNamesList(void) const;
        size_t                              getServerNameCount() const;
        std::string                         getErrorFile(int error_page_index) const;
        std::string                         getErrorRoot(int error_page_index) const;
        size_t                              getErrorPageCount() const;
        unsigned long                       getClientMaxBodySize() const;
        const std::vector<std::string>&     getDefaultIndices() const;
        std::string                         getDefaultIndexFile(size_t index_file) const;
        size_t                              getDefaultIndexCount() const;
        std::string                         getDefaultRoot() const;
        const std::vector<LocationConfig>&  getLocations() const;
        const LocationConfig&               getLocation(size_t loc_index) const;
        const std::vector<std::string>&     getLocationIndices(size_t loc_index) const;
        size_t                              getLocationCount() const;
        std::string                         getLocationPath(size_t loc_index) const;
        std::string                         getLocationRoot(size_t loc_index) const;
        std::string                         getLocationIndexFile(size_t loc_index, size_t index_file) const;
        size_t                              getLocationIndexCount(size_t loc_index) const;
        bool                                getAutoIndex(size_t loc_index) const;
        bool                                getRedirect(size_t loc_index) const;
        std::string                         getRedirectUrl(size_t loc_index) const;
        size_t                              getRedirectCode(size_t loc_index) const;
        size_t                              getMethodsSize(size_t loc_index) const;
        std::set<std::string>               getMethods(size_t loc_index) const;
        std::string                         getMethod(size_t loc_index, size_t method_index) const;
        std::string                         getCgiExtensions(size_t loc_index, size_t cgi_extension_index) const;
        size_t                              getCgiExtensionCount(size_t loc_index) const;
        std::string                         getUploadStore(size_t loc_index) const;
};

#endif

