#ifndef Core_HPP
# define Core_HPP

/* Colors */
#define RED "\033[31m"
#define GREY "\033[90m"
#define BLUE "\033[34m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "Server.h"
#include "../http/HTTPRequest.h"
#include "../http/HTTPResponse.h"

const std::string   CGI_PATH = "/home/nlence-l/coding/webserv/cgi-bin/";
const int           MAX_EVENTS = 50; // num clients
const int           MAX_REQUEST_SIZE = 40960;

class Core
{
private:
    std::map<int, int>  _psocket;
    std::vector<int>    _server_sockets;
    std::vector<int>    _client_sockets;
    std::vector<int>    _ports;
    // std::vector<Server> _clusters;
    std::map<std::string, std::string> _routes;
    int                 _max_clients;
    
public:
    Core();
    ~Core();

    // Getters
    int         get_server_sockets_size();
    int         get_client_sockets_size();
    int         get_server_socket(int i);
    int         get_client_socket(int i);
    std::string get_file_path(std::string filename);

    // Member functions
    std::string get_file_name(std::string r);
    void    generate_routes();
    void    add_client_socket_to_vector(int socket);
    void    remove_client_socket_from_vector(int socket);
    int     get_psock(int socket);
    void    add_port(int port);
    void    bind_ports();
    // void    add_cluster(const Server &cluster);
    void    map_sock(int socket, int port);
    void    unmap_sock(int socket);
    void    close_sock();

    class CantFindPort : public std::exception {
        public: virtual const char *what() const throw();
    };

    class CantCreateSocket : public std::exception {
        public: virtual const char *what() const throw();
    };

    class CantBindSocket : public std::exception {
        public: virtual const char *what() const throw();
    };

    class ListeningError : public std::exception {
        public: virtual const char *what() const throw();
    };
};

template<typename T>
void    printMsg(T msg, std::string color, int endl) {
    std::cout << color << msg << RESET;
    for (int i = 0; i < endl; i++)
        std::cout << std::endl;
}


// utils.cpp
int         set_non_blocking(int fd);
std::string get_fileContent(const std::string& filename);
std::string get_contentType(const std::string& filename);
std::string execute_cgi(const std::string& script_name);

#endif