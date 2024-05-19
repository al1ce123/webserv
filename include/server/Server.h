#ifndef SERVER_HPP
# define SERVER_HPP

# include "Core.h"
# include "Location.h"

# include <fstream>
# include <stdlib.h>

class Server
{
public:
    // Constructors
    Server();
    Server(const Server &src);
    ~Server();
    Server& operator=(const Server &rhs);

    // Member functions
    void parser(std::string file);
    int parse_listen_line(std::string line);
    void parse_server_name_line(std::string line);
    Location* parse_location_line(std::ifstream& file, std::string line);
    void parse_index_line(std::string line);
    std::string parse_root_line(std::string line);
    void parse_error_page_line(std::string line);

private:
    std::map<std::string, std::string> _sockets; // parse the listen directive
    std::vector<std::string> _server_name; // parse the server_name directive
    std::size_t _max_body_size;
    std::vector<std::string> _index; // files to look for when a directory is requested
    std::string _root; //  root directory from which webserv should serve files for a given server block
    std::map<std::string, std::string> _error_pages; /* [http status code/file_name], with nginx config file you can 
                                                       specify multiple status code per line, but in our webserv
                                                       config file you have to specify one status code and one file 
                                                       per line for the sake of simplicity */ 
    Location** _locations;
};

#endif