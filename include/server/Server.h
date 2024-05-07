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

private:
    std::map<std::string, std::string> _sockets; // parse the listen directive
    std::map<std::string, std::string> _server_names; // parse the server_name directive
    bool _directory_listing;
    std::size_t _max_body_size;
    Location _locations[MAX_LOCATIONS];
};

#endif