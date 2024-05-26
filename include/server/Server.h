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
    int parseListenLine(std::string line);
    void parseServerNameLine(std::string line);
    Location* parseLocationLine(std::ifstream& file, std::string line);
    void parseIndexLine(std::string line);
    std::string parseRootLine(std::string line);
    void parseErrorPageLine(std::string line);
    void resize(size_t new_capacity);
    void addLocation(Location* location);
    std::string parseClientMaxBodySizeLine(std::string line);
    

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
    size_t _size;
    size_t _capacity;
};

#endif
