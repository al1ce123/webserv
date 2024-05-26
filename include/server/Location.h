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
    std::string getTheRoute(std::string line);
    std::string getDirective(std::string line);
    std::string getRoot(std::string line);
    void getIndex(std::string line);
    void getMethods(std::string line);
    std::string getRedir(std::string line);
    std::string setAutoindexToTrue(std::string line);
    std::string getMaxClientBodySize(std::string line);

private:
    std::size_t _max_body_size;
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