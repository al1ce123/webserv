#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_LOCATIONS 10

# include "Core.h"
# include "Location.h"

# include <fstream>
# include <stdlib.h>

class Server
{
public:
    Server();
    Server(const Server &src);
    ~Server();
    Server& operator=(const Server &rhs);

    // Member functions
    void parser(std::string file);
    int parse_listen_line(std::string line);
    Location* parse_location_line(std::ifstream& file, std::string route);

private:
    std::map<std::string, std::string> _sockets; // parse the listen directive
    std::map<std::string, std::string> _server_names; // parse the server_name directive
    bool _directory_listing;
    std::size_t _max_body_size;
    std::vector<std::string> _index; // files to look for when a directory is requested
    std::string _root; //  root directory from which webserv should serve files for a given server block
    std::map<std::string, std::string> error_pages;
    Location _locations[MAX_LOCATIONS];
};

#endif