/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 20:51:48 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 09:54:05 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Core.hpp"

Core::Core() : _maxClients(100) {(void)this->_maxClients;}

Core::~Core() {}

void Core::core_parser(std::string& file) {
    std::ifstream configFile(file);
    
    if (!configFile) {
        std::cerr << "Failed to open configuration file" << '\n';
    }
    Server* tmp = NULL;
    std::string line;
    size_t pos;
    
    while (std::getline(configFile, line)) {
        pos = line.find("server {");
        if (pos != std::string::npos) {

            tmp = new Server();

            tmp->parser(configFile);

            /* testing */
            // tmp->printInfos();
            // std::cout << "\n\n";

            this->_clusters.push_back(tmp);

        } else {
            continue;
        }
    }
    configFile.close();
    // std::cout << "TESTING: " << this->_clusters.size() << " server block detected" << '\n';
}

/***** getters *****/
// int     Core::get_socket(int i) {return this->_sockets[i];}
const   std::map<int, sockaddr_in>& Core::get_clients() const {
    return this->_clients;
}

const   std::map<int, sockaddr_in>& Core::get_servers() const {
    return this->_servers;
}

/***** member functions *****/
int    Core::bind_ports(std::vector<pollfd>& poll_fds) {
    for (size_t i = 0; i < this->_clusters.size(); i++) {
        std::vector<int> temp;
        temp = this->_clusters[i]->getPorts();
        this->_ports.insert(this->_ports.end(), temp.begin(), temp.end());
    }

    for (size_t i = 0; i < this->_ports.size(); i++) {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            std::cerr << "Socket error: " << strerror(errno) << std::endl;
            return 2;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(this->_ports[i]);

        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            std::cerr << "Bind error: " << strerror(errno) << std::endl;
            close(server_fd);
            return 3;
        }

        if (listen(server_fd, 10) == -1) {
            std::cerr << "Listen error: " << strerror(errno) << std::endl;
            close(server_fd);
            return 4;
        }

        this->_servers[server_fd] = server_addr;
        pollfd server_socket = { server_fd, POLLIN, 0 };
        poll_fds.push_back(server_socket);
        
        if (this->_ports.empty()) {
            std::cerr << "No ports to bind." << std::endl;
            return 1;
        }
        
        std::cout << "Server listening on port " << this->_ports[i] << std::endl;
    }
    
    return 0;
}

void    Core::close_all_sockets() {
    std::map<int, sockaddr_in>::iterator it;

    for (it = this->_servers.begin(); it != this->_servers.end(); ++it) {
        close(it->first);
    }
    for (it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        close(it->first);
    }
}

void Core::add_client(int client_fd, const sockaddr_in& client_addr) {
    this->_clients[client_fd] = client_addr;
}

bool Core::is_server_socket(int sockfd) {
    std::map<int, sockaddr_in>::iterator it;
    for (it = this->_servers.begin(); it != this->_servers.end(); it++) {
        if (it->first == sockfd) {
            return true;
        }
    }
    return false;
}

bool Core::is_client_socket(int sockfd) {
    std::map<int, sockaddr_in>::iterator it;
    for (it = this->_clients.begin(); it != this->_clients.end(); it++) {
        if (it->first == sockfd) {
            return true;
        }
    }
    return false;    
}

int Core::webserv(void) {
    std::vector<pollfd> poll_fds;
    std::map<int, std::string> responses;
    
    this->bind_ports(poll_fds);

    while (42) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }

        for (size_t i = 0; i < poll_fds.size(); i++) {
            if ((this->is_server_socket(poll_fds[i].fd)) && (poll_fds[i].revents & POLLIN)) {
                // Accept new client connection
                struct sockaddr_in client_addr;
                socklen_t addr_size = sizeof(client_addr);
                
                int client_sockfd = accept(poll_fds[i].fd, (struct sockaddr*)&client_addr, &addr_size);
                if (client_sockfd < 0) {
                    std::cerr << "Accept error: " << strerror(errno) << std::endl;
                    return 1;
                }
                pollfd client_pollfd = { client_sockfd, POLLIN, 0 };
                poll_fds.push_back(client_pollfd);
                this->add_client(client_sockfd, client_addr);
            } else if ((this->is_client_socket(poll_fds[i].fd)) && (poll_fds[i].revents & POLLIN)) {
                std::string http_request;
                char        buffer;
                bool        stop = true;
                bool        close_sock = false;
                Request     *req = new Request;
                Response    *res = new Response;

                while (42) {
                    ssize_t bytes = recv(poll_fds[i].fd, &buffer, 1, 0);

                    if (bytes <= 0) {
                        if (bytes < 0)
                            res->set_status("500", " Internal server error");
                        else
                            res->set_status("400", " Bad Request");
                        close_sock = true;
                        break;
                    }
                    
                    http_request.push_back(buffer);
                    size_t head_end = http_request.find("\r\n\r\n");
                    if (head_end != std::string::npos) {
                        try {
                            req->set_header(http_request);
                            this->check_req(req, res, poll_fds[i].fd);
                            stop = false;
                        } catch (Core::InterServError &e) {
                            std::cout << e.what() << std::endl;
                            close_sock = true;
                        } catch (std::exception &e) {
                            std::cout << e.what() << std::endl;
                        }
                        break;
                    }
                }
                
                if (!stop) {
                    try {
                        this->req_body(poll_fds[i].fd, req, res);
                        this->handle_req(req, res);
                    } catch (Core::BodyRecvError &e) {
                        std::cout << e.what() << std::endl;
                        close_sock = true;
                    } catch (std::exception &e) {
                        std::cout << e.what() << std::endl;
                    }
                }

                if (!close_sock) {
                    responses[poll_fds[i].fd] = res->get_response();
                    poll_fds[i].events = POLLOUT;
                } else {
                    delete req;
                    delete res;
                    close(poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                    i--;
                }
            } else if ((this->is_client_socket(poll_fds[i].fd)) && (poll_fds[i].revents & POLLOUT)) {
                // Send response to client
                std::string &response = responses[poll_fds[i].fd];
                ssize_t bytes_sent = send(poll_fds[i].fd, response.c_str(), response.size(), 0);
                if (bytes_sent < 0) {
                    std::cerr << "Send error: " << strerror(errno) << std::endl;
                } else {
                    response.erase(0, bytes_sent);
                    if (response.empty()) {
                        responses.erase(poll_fds[i].fd);
                        close(poll_fds[i].fd);
                        poll_fds.erase(poll_fds.begin() + i);
                        i--;
                    }
                }
            }
        }
    }
    
    this->close_all_sockets();
    return 0;
}


void    Core::check_req(const Request *req, Response *res, int client_sockfd) {
    Server*      match_clust = match_cluster(req, client_sockfd);
    bool        stop = false;

    if (!match_clust) {
        res->set_status("500", " Internal server error");
        stop_req(res, match_clust, req->get_uri());
        throw Core::InterServError();
    }
    
    if (match_clust->hasMethods()) {
        for (size_t i = 0; i < match_clust->getMethodsSize(); i++) {
            if (req->get_method() != match_clust->getMethod(i)) { 
                res->set_status("405", " Forbidden request method");
                stop = true;
            } else if (req->get_method() == match_clust->getMethod(i)) {
                stop = false;
                break;
            }
        }
    }
    
    if ((req->get_contLen() > 0 || req->chunked() == true)
        && match_clust->getMaxBodySize() > 0 // max body size
        && req->get_contLen() > match_clust->getMaxBodySize()) {
            res->set_status("413", " Payload too large");
            stop = true;
    }

    stop = check_method(req, res, match_clust);
    
    if (stop == true) {
        stop_req(res, match_clust, req->get_uri());
        throw Core::NotMatchingConf();
    } else
        res->set_status("", "");
}

/*
The match_cluster function iterates over all our servers
in _clusters to find the best server block match and returns
a pointer to Server previously allocated (in core_parser).
I added the client fd because we need its ip addr and port.
*/
Server* Core::match_cluster(const Request *req, int client_sockfd) {
    std::vector<size_t> candidates;
    Server *result = nullptr;

    char temp[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &this->_clients[client_sockfd].sin_addr, temp, INET_ADDRSTRLEN) == nullptr) {
        std::cerr << "Inet_ntop error: " << strerror(errno) << std::endl;
        return nullptr;
    }
    int client_port = ntohs(this->_clients[client_sockfd].sin_port);

    std::string client_ip(temp);

    for (size_t i = 0; i < this->_clusters.size(); i++) {
        std::map<std::string, std::vector<std::string>> sockets = this->_clusters[i]->getSockets();

        for (std::map<std::string, std::vector<std::string>>::iterator it = sockets.begin(); it != sockets.end(); it++) {
            std::vector<int> ports;
            int port;

            for (size_t j = 0; j < it->second.size(); j++) {
                std::istringstream iss(it->second[j]);
                if (!(iss >> port)) {
                    std::cerr << "Invalid port: " << it->second[j] << std::endl;
                    continue;
                }
                ports.push_back(port);
            }

            if (client_ip == it->first || client_ip == "0.0.0.0") {
                for (size_t j = 0; j < ports.size(); j++) {
                    if (client_port == ports[j]) {
                        candidates.push_back(i);
                        break;
                    }
                }
            }
        }
    }

    if (candidates.empty()) {
        std::cerr << "No matching server block found for client: " << client_ip << ":" << client_port << std::endl;
        return nullptr;
    }

    std::vector<std::string> server_names;
    if (candidates.size() > 1) {
        for (size_t i = 0; i < candidates.size(); i++) {
            server_names = this->_clusters[candidates[i]]->getServerName();
            for (size_t j = 0; j < server_names.size(); j++) {
                if (req->get_host() == server_names[j]) {
                    result = this->_clusters[candidates[i]];
                    return result;
                }
            }
        }
    }

    result = this->_clusters[candidates[0]];
    return result;
}

bool    Core::check_method(const Request *req, Response *res, Server *cluster) {
    bool ret = false;
    std::string path = req->get_uri();

    while (true) {
        size_t  pos = path.find("/", 1);
        if (pos == std::string::npos)
            break;
        std::string tmp = path.substr(0, pos);
        path = path.substr(tmp.length(), (path.length() - tmp.length()));

        for (size_t i = 0; i < cluster->getLocationsSize(); i++) { // location vector size
            if (tmp.find(cluster->getLocation(i)->getRoute()) != std::string::npos) {
                if (cluster->getLocation(i)->hasMethods()) { 
                    for (size_t j = 0; j < cluster->getLocation(i)->getMethodsSize(); j++) {
                        if (req->get_method() != cluster->getLocation(i)->getMethod(i)) {
                            res->set_status("405", " Forbidden request method");
                            ret = true;
                        } else if (req->get_method() == cluster->getLocation(i)->getMethod(i)) {
                            ret = false;
                            break;
                        }
                    }
                }
                if ((req->get_contLen() > 0 || req->chunked() == true)
                    && cluster->getLocation(i)->getMaxBodySize() > 0
                    && req->get_contLen() > cluster->getLocation(i)->getMaxBodySize()) {
                        res->set_status("405", " Forbidden request method");
                        ret = true;
                } else if ((req->get_contLen() > 0 || req->chunked() == true)
                    && cluster->getLocation(i)->getMaxBodySize() > 0
                    && req->get_contLen() < cluster->getLocation(i)->getMaxBodySize())
                    ret = false;
            }
        }
    }
    return ret;
}

void    Core::req_body(int socket, Request *req, Response *res) { // is socket the client's socket ???
    Server  *match_clust = match_cluster(req, socket);
    size_t  max_size = 0;

    if (!match_clust) {
        res->set_status("500", " Internal server error");
        stop_req(res, match_clust, req->get_uri());
        throw Core::InterServError();
    }
    max_size = max_body_size(req, match_clust);
    
    if (req->has_body()) {
        if (req->has_body() == 1)
            recv_chunked(socket, req, res, max_size);
        else if (req->has_body() == 2)
            recv_multipart(socket, req, res, max_size);
        else
            recv_contLen(socket, req, res, max_size);
    }
}

void    Core::stop_req(Response *res, Server *cluster, std::string path) {
    std::string err_path;

    if (cluster) {
        err_path = cluster->getErrorPage(res->get_status(), err_path);

        while (true) {
            size_t  pos = path.find("/", 1);
            if (pos == std::string::npos)
                break;
            std::string tmp = path.substr(0, pos);
            path = path.substr(tmp.length(), (path.length() - tmp.length()));

            for (size_t i = 0; i < cluster->getLocationsSize(); i++) {
                err_path = cluster->getLocation(i)->getErrorPage(res->get_status(), err_path);
            }
        }
        if (!err_path.empty()) {
            res->set_body(get_fileContent(err_path));
            res->set_file(err_path);
        } else {
            res->set_body(get_fileContent(WEBSITE_PATH + "status/" + res->get_status() + ".html"));
            res->set_file(".html");
        }
    } else {
        res->set_body(get_fileContent(WEBSITE_PATH + "status/500.html"));
        res->set_file(".html");
    }
    res->set_head();
}

void    Core::handle_req(const Request *req, Response *res) {
    std::string filename = req->get_uri();
    if (req->get_method() == "GET") {
        if (filename.empty() || filename == "/")
            res->set_file(WEBSITE_PATH + "index.html");
        else
            res->set_file(WEBSITE_PATH + filename);
        
        if (get_fileContent(res->get_file()).empty()) {
            res->set_status("404", " Not Found");
            res->set_body(get_fileContent(WEBSITE_PATH + "status/404.html"));
        } else {
            res->set_status("200", " OK");
            res->set_body(get_fileContent(res->get_file()));
        }
    } else {
        res->set_status("404", " Not Found");
        res->set_body(get_fileContent(WEBSITE_PATH + "status/404.html"));
    }
    res->set_head();
}

/***** exceptions *****/
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

const char *Core::NotMatchingConf::what() const throw() {
    return ("\033[31mERROR\033[0m: Can't continue request for config reason");
}

const char *Core::InterServError::what() const throw() {
    return ("\033[31mERROR\033[0m: The server encountered an internal error");
}

const char *Core::BodyRecvError::what() const throw() {
    return ("\033[31mERROR\033[0m: Error receiving request's body");
}

