/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 20:52:00 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 16:15:03 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     Core_HPP
# define    Core_HPP

#define RED "\033[31m"
#define GREY "\033[90m"
#define BLUE "\033[34m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"

#define CHUNK_SIZE 4096

# include <iostream>
# include <sstream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <algorithm>
# include <string.h>
# include <errno.h>
# include <arpa/inet.h>

# include "../http/Request.hpp"
# include "../http/Response.hpp"
# include "Server.hpp"
# include "Location.hpp"
# include "../http/Part.hpp"

const std::string WEBSITE_PATH = "./www/";
const std::string CGI_PATH = "./cgi-bin/";

class Core
{
private:
    std::map<int, std::pair<std::string, int>>          _servers; // server_sockfd, binded to this ip/port
    std::map<int, std::pair<std::string, int>>          _clients; // client_sockfd, the client did a request to this ip/port
    std::vector<int>                                    _ports;
    std::vector<Server*>                                _clusters;
    int                                                 _maxClients;

public:
    Core();
    ~Core();

    /***** getters *****/
    const   std::map<int, std::pair<std::string, int>> & get_clients() const;
    const   std::map<int, std::pair<std::string, int>> & get_servers() const;

    /***** member functions *****/
    int                     bind_ports(std::vector<pollfd>& poll_fds);
    void                    close_all_sockets();
    void                    add_client(int client_fd, std::string requested_ip, int requested_port);
    bool                    is_server_socket(int sockfd);
    bool                    is_client_socket(int sockfd);
    void                    core_parser(std::string& file);
    int                     webserv(void);
    Server                  *match_cluster(const Request *req, int client_sockfd);
    void                    check_req(const Request *req, Response *res, int client_sockfd);
    bool                    check_method(const Request *req, Response *res, Server *cluster);
    void                    req_body(int socket, Request *req, Response *res);
    void                    stop_req(Response *res, Server *cluster, std::string path);
    void                    handle_req(const Request *req, Response *res);

    /***** exceptions *****/
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
    class NotMatchingConf : public std::exception {
        public: virtual const char *what() const throw();
    };
    class InterServError : public std::exception {
        public: virtual const char *what() const throw();
    };
    class BodyRecvError : public std::exception {
        public: virtual const char *what() const throw();
    };
    
    
};

template<typename T>
void                        printMsg(T msg, std::string color, int endl) {
    std::cout << color << msg << RESET;
    for (int i = 0; i < endl; i++)
        std::cout << std::endl;
}

/***** utils.cpp *****/
std::string                 exec_cgi(const std::string& script_name, int timeout_seconds);
std::string                 get_fileContent(const std::string& filename);
std::string                 read_line(int client_socket, Response *res);
size_t                      max_body_size(const Request *req, Server *cluster);

/***** recv_body.cpp *****/
void                        recv_multipart(int socket, Request *req, Response *res, size_t mbs);
void                        recv_contLen(int socket, Request *req, Response *res, size_t mbs);
void                        recv_chunked(int socket, Request *req, Response *res, size_t mbs);
void                        part_headers(int client_socket, Part *part, Response *res);

#endif