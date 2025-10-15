#include "../includes/ServerWrapper.hpp"

ServerWrapper::ServerWrapper() {}

ServerWrapper::ServerWrapper(const ServerConfig& cfg) : config(&cfg) {}

ServerWrapper::ServerWrapper(const ServerWrapper& src) {*this = src;}

ServerWrapper& ServerWrapper::operator=(const ServerWrapper& src) {
    
    if (this != &src) {

        this->config = src.config;
        return (*this);
    }
    return (*this);
};

ServerWrapper::~ServerWrapper() {}


std::string             ServerWrapper::getIps(size_t ip_index) const {

    if (!config || ip_index >= config->ips_and_ports.size()) return ("");
    return (config->ips_and_ports[ip_index].first);
}


size_t                  ServerWrapper::getIpCount() const {

    return (config->ips_and_ports.size());
}


uint16_t                     ServerWrapper::getPorts(size_t port_index) const {

    if (!config || port_index >= config->ips_and_ports.size()) return (-1);
    return (config->ips_and_ports[port_index].second);
}


size_t                  ServerWrapper::getPortCount() const {

    if (!config) return (0);
    return (config->ips_and_ports.size());
}

std::string             ServerWrapper::getServerName(size_t server_name_index) const {

    if (!config || server_name_index >= config->server_names.size()) return ("");
    return (config->server_names[server_name_index]);
}

std::vector<std::string> ServerWrapper::getServerNamesList(void) const {

    return (config->server_names);
}

size_t                  ServerWrapper::getServerNameCount() const {

    if (!config) return (0);
    return (config->server_names.size());
}

size_t            ServerWrapper::getCountIpPorts () {
    
    return (config->ips_and_ports.size());
}

std::string             ServerWrapper::getErrorRoot(int error_page_index) const {

    if (!config) return ("");
    std::map<int, std::pair<std::string, std::string> >::const_iterator it;
    it = config->error_pages.find(error_page_index);
    if (it != config->error_pages.end())
        return (it->second.first);
    return ("");
}

std::string             ServerWrapper::getErrorFile(int error_page_index) const {

    if (!config) return ("");
    std::map<int, std::pair<std::string, std::string> >::const_iterator it;
    it = config->error_pages.find(error_page_index);
    if (it != config->error_pages.end())
        return (it->second.second);
    return ("");
}

size_t                  ServerWrapper::getErrorPageCount() const {

    if (!config) return (0);
    return (config->error_pages.size());
}


uint64_t                  ServerWrapper::getClientMaxBodySize() const {

    if (!config) return (0);
    return (config->client_max_body_size);
}


size_t                              ServerWrapper::getDefaultIndexCount() const {

    if (!config || config->default_indices.empty())
        return (0);
    return (config->default_indices.size());
}

std::string                         ServerWrapper::getDefaultIndexFile(size_t index_file) const {

    if (!config || index_file >= config->default_indices.size()) 
        return ("");

    const std::vector<std::string>& indices = config->default_indices;

    return (indices[index_file]);
}

std::string                         ServerWrapper::getDefaultRoot() const {

    if (!config || config->default_root.empty()) return ("");
    return (config->default_root);
}

const std::vector<LocationConfig>& ServerWrapper::getLocations() const {

    if (!config)
        throw std::out_of_range("Invalid location index");
    return (config->locations);
}


const LocationConfig&   ServerWrapper::getLocation(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size())
        throw (std::out_of_range("Invalid location index"));

    return (config->locations[loc_index]);
}


size_t                  ServerWrapper::getLocationCount() const {

    if (!config) return (0);
    return (config->locations.size());
}


std::string             ServerWrapper::getLocationPath(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) return ("");
    return (config->locations[loc_index].path);
}



std::string             ServerWrapper::getLocationRoot(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size() || config->locations[loc_index].root.empty()) return ("");
    return (config->locations[loc_index].root);
}

size_t ServerWrapper::getLocationIndexCount(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) 
        return (0);

    return (config->locations[loc_index].indices.size());
}

std::string ServerWrapper::getLocationIndexFile(size_t loc_index, size_t index_file) const {

    if (!config || loc_index >= config->locations.size()) 
        return ("");

    const std::vector<std::string>& indices = config->locations[loc_index].indices;

    if (index_file >= indices.size()) 
        return ("");

    return (indices[index_file]);
}

const std::vector<std::string>&      ServerWrapper::getDefaultIndices() const {
    
    return (config->default_indices);
}

const std::vector<std::string>&     ServerWrapper::getLocationIndices(size_t loc_index) const {

    return (config->locations[loc_index].indices);
}

bool                    ServerWrapper::getAutoIndex(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) return (false);
    return (config->locations[loc_index].auto_index);
}

bool                   ServerWrapper::getRedirect(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) return (false);
    return (config->locations[loc_index].has_redirect);
}

std::string            ServerWrapper::getRedirectUrl(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) return (0);
    return (config->locations[loc_index].redirect_url);
}

size_t             ServerWrapper::getRedirectCode(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) return (0);
    return (config->locations[loc_index].redirect_code);
}

size_t                ServerWrapper::getMethodsSize(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size() || config->locations[loc_index].methods.empty())
        return (0);
    return (config->locations[loc_index].methods.size());
}

std::set<std::string>  ServerWrapper::getMethods(size_t loc_index) const {

    static std::set<std::string> methods;
    if (!config || loc_index >= config->locations.size())
        return (methods);
    methods = config->locations[loc_index].methods;
    return (methods);
}

std::string             ServerWrapper::getMethod(size_t loc_index, size_t method_index) const {
    
    if (!config || loc_index >= config->locations.size()) return ("");
    const std::set<std::string>& methods = config->locations[loc_index].methods;

    if (method_index >= methods.size()) return ("");
    std::set<std::string>::const_iterator it = methods.begin();
    std::advance(it, method_index);
    return (*it);
}


std::string             ServerWrapper::getCgiExtensions(size_t loc_index, size_t cgi_extension_index) const {

    if (!config || loc_index >= config->locations.size()) return ("");
    const std::set<std::string>& extensions = config->locations[loc_index].cgi_extensions;

    if (cgi_extension_index >= extensions.size()) return "";
    std::set<std::string>::const_iterator it = extensions.begin();
    std::advance(it, cgi_extension_index);
    return (*it);
}


size_t                  ServerWrapper::getCgiExtensionCount(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) return (0);
    return (config->locations[loc_index].cgi_extensions.size());
}


std::string             ServerWrapper::getUploadStore(size_t loc_index) const {

    if (!config || loc_index >= config->locations.size()) return ("");
    return (config->locations[loc_index].upload_store);
}

