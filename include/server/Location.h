#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Core.h"

class Location
{
public:
    Location(std::string route);
    Location(const Location &src);
    ~Location();
    Location &operator=(const Location &rhs);

private:
    std::string _route; // default route
    std::string _root; // directory where to serve file that match the route
    std::vector<std::string> _index; // vector to store default files to serve when the request is a directory
    std::vector<std::string> _methods; // list of accepted methods for this route
    std::string _redir; // uri to perform the redirection for the specified route
    bool _autoindex; // boolean variable to enable or not directory listing
    // CGI
};

#endif