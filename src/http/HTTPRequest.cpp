#include "../../include/http/HTTPRequest.h"

HTTPRequest::HTTPRequest(std::string &request) {
    size_t pos = request.find("\r\n\r\n");

    this->_content_len = 0;
    if (pos != std::string::npos) {
        this->_header = request.substr(0, pos);
        this->_body = request.substr(pos + 4);
        parse_header(this->_header);
    } else {
        this->_header = request;
    }
}

HTTPRequest::~HTTPRequest() {}

void    HTTPRequest::parse_header(const std::string& header) {
    std::istringstream iss(header);
    std::string first_line;

    // Get the first line of the header
    std::getline(iss, first_line, '\n');

    // Extract method and URI from the first line
    std::istringstream first_line_stream(first_line);
    first_line_stream >> this->_method >> this->_uri;

    // Parse the rest of the header fields
    std::string line;
    while (std::getline(iss, line, '\n')) {
        // Find the position of the colon (":") character
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            // Extract the field name and value
            std::string field = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Trim leading and trailing whitespace from the value
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            // Store the field value in the appropriate member variable
            if (field == "Accept") {
                this->_content_type = value;
            } else if (field == "Content-Length") {
                this->_content_len = atoi(value.c_str());
            }
        }
    }
}

void    HTTPRequest::test_parser() {
    std::cout << "method: " << this->_method << std::endl;
    std::cout << "URI: " << this->_uri << std::endl;
    std::cout << "content type: " << this->_content_type << std::endl;
    std::cout << "content lenght: " << this->_content_len << std::endl;
}