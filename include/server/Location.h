#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Core.h"

class Location
{
public:
    // Constructors
    Location();
    Location(const Location &src);
    ~Location();
    Location& operator=(const Location &rhs);

    // Member functions
    std::string get_the_route(std::string line);
    std::string get_directive(std::string line);
    std::string get_root(std::string line);
    void get_index(std::string line);
    void get_methods(std::string line);
    std::string get_redir(std::string line);
    std::string set_autoindex_to_true(std::string line);

private:
    std::string _route; // default route
    std::string _root; // directory where to serve file that match the route
    std::vector<std::string> _index; /* vector to store default files to serve when a directory is requested
                                       /!\ location blocks also have an index directive that behave the same way
                                       location's index directive always overwrites server's index directives */ 
    std::vector<std::string> _methods; // list of accepted methods for this route
    std::map<std::string, std::string> _redir; // [status code/uri] to perform the redirection for the specified route
    bool _autoindex; // boolean variable to enable or not directory listing, defined as fasle by default
    // CGI
};

#endif