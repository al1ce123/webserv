#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "../server/Core.h"

class HTTPRequest
{
private:
    std::string _header;
    std::string _body;
    std::string _method;
    std::string _uri;
    std::string _content_type;
    int         _content_len;

    std::map<std::string, std::string> _formData; // user to store POST request body parameters
    
public:
    HTTPRequest(std::string &request);
    ~HTTPRequest();

    void    parse_header(const std::string &header);
    void    test_parser();
};

#endif