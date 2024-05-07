#include "../../include/server/Core.h"

Core::Core() : _max_clients(10) {
    (void)this->_max_clients;
}

Core::~Core() {}

// Getters
int     Core::get_server_sockets_size() {
    return this->_server_sockets.size();
}

int     Core::get_client_sockets_size() {
    return this->_client_sockets.size();
}

int     Core::get_server_socket(int i) {
    return this->_server_sockets[i];
}

int     Core::get_client_socket(int i) {
    return this->_client_sockets[i];
}

std::string Core::get_file_path(std::string filename) {
    size_t pos = filename.find_last_of(".");
    std::string extension = filename.substr(pos + 1);
    return this->_routes[extension];
}

// Member functions
std::string Core::get_file_name(std::string r) {
    std::string res;
    res = r.substr(4, r.find("HTTP/1.1") - 4 - 1);
    std::size_t pos = res.find_last_of("/");
    res = res.substr(pos);
    if (res == "/") {
        return "";
    }
    return res.substr(1);
}

void Core::generate_routes(void) {
    std::string user = getenv("USER");
    std::string path_to_webserv_from_home = "/coding/webserv"; // ne pas rajouter de '/' apres le webserv

    this->_routes["html"] = "/home/" + user + path_to_webserv_from_home + "/www/pages/";
    this->_routes["css"] = "/home/" + user + path_to_webserv_from_home + "/www/css/";

    // Most common image file formats
    this->_routes["png"] = "/home/" + user + path_to_webserv_from_home + "/www/images/";
    this->_routes["jpg"] = "/home/" + user + path_to_webserv_from_home + "/www/images/";
    this->_routes["jpeg"] = "/home/" + user + path_to_webserv_from_home + "/www/images/";
    this->_routes["webp"] = "/home/" + user + path_to_webserv_from_home + "/www/images/";

    this->_routes["cgi"] = "/home/" + user + path_to_webserv_from_home + "/cgi-bin/";
}

void    Core::add_client_socket_to_vector(int socket) {
    this->_client_sockets.push_back(socket);
}

void    Core::remove_client_socket_from_vector(int socket) {
    for (std::vector<int>::iterator it = this->_client_sockets.begin(); it != this->_client_sockets.end(); ++it) {
        if (*it == socket) {
            this->_client_sockets.erase(it);
            return;
        }
    }
}

int     Core::get_psock(int socket) {
    if (_psocket.empty() || _psocket.find(socket) == _psocket.end())
        throw CantFindPort();
    else {
        return _psocket[socket];
    }
}

void    Core::add_port(int port) {
    //add check
    this->_ports.push_back(port);
}

void    Core::bind_ports() {
    for (size_t i = 0; i < this->_ports.size(); i++) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            throw Core::CantCreateSocket();
        }

        // Set socket to non-blocking mode
        if (set_non_blocking(sockfd) == -1) {
            exit(EXIT_FAILURE);
        }

        sockaddr_in as;
        as.sin_family = AF_INET;
        as.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        as.sin_port = htons(this->_ports[i]);
        if (bind(sockfd, (sockaddr*)&as, sizeof(as)) < 0) {
            throw Core::CantBindSocket();
        }
        if (listen(sockfd, 10) < 0) {
            throw Core::ListeningError();
        }
        this->map_sock(sockfd, this->_ports[i]);
        this->_server_sockets.push_back(sockfd);
    }

    std::cout << "Server listening on ";
    for (size_t i = 0; i < this->_ports.size(); i++) {
        std::cout << this->_ports[i];
        if (i + 1 == this->_ports.size()) {
            std::cout << "\n";
        }
    }
}

// void    Core::add_cluster(const Server &cluster) {
//     //Add check
//     this->_clusters.push_back(cluster);
// }

void    Core::map_sock(int socket, int port) {
    this->_psocket[socket] = port;
}

void    Core::unmap_sock(int socket) {
    this->_psocket.erase(socket);
}

void    Core::close_sock() {
    for (size_t i = 0; i < this->_server_sockets.size(); i++) {
        close(this->_server_sockets[i]);
    }
    for (size_t i = 0; i < this->_client_sockets.size(); i++) {
        close(this->_client_sockets[i]);
    }
}

// Exceptions
const char *Core::CantFindPort::what() const throw() {
    return ("\033[31mERROR\033[0m: Can't find port for this socket");
}

const char *Core::CantCreateSocket::what() const throw() {
    return ("\033[31mERROR\033[0m: Can't create socket");
}

const char *Core::CantBindSocket::what() const throw() {
    return ("\033[31mERROR\033[0m: Can't bind the socket to port");
}

const char *Core::ListeningError::what() const throw() {
    return ("\033[31mERROR\033[0m: Failed listening on port");
}