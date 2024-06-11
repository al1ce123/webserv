/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 15:41:53 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 09:13:03 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http/Request.hpp"

Request::Request() : _port(0), _content_len(0), _chunked(false) {}

Request::~Request() {}

/***** getters *****/
std::string Request::get_uri(void) const {return this->_uri;}

std::string Request::get_bound(void) {return this->_boundary;}

size_t      Request::get_contLen(void) const {return this->_content_len;}

int         Request::get_port(void) const {return this->_port;}

std::string                         Request::get_ip_addr(void) const {return this->_ip_addr;}


/***** CGI getters *****/
std::string Request::get_content_length(void) const {

    if (this->_headerMap.count("Content-Length")) {
        return this->_headerMap.at("Content-Length");
    } else {
        return "";
    }
}

std::string Request::get_content_type(void) const {
    if (this->_headerMap.count("Content-Type")) {
        return this->_headerMap.at("Content-Type");
    } else {
        return "";
    }
}

std::string Request::get_query_string(void) const {
    if (this->_method == "GET") {
        size_t pos = _header.find("?");
        if (pos != std::string::npos) {
            size_t end = _header.find(" ", pos);
            return _header.substr(pos + 1, end - pos - 1).c_str();
        } else {
            return "";
        }
    } else if (this->_method == "POST") {
        return this->_body.c_str();
    } else {
        return "";
    }
}

std::string Request::get_host(void) const {
    if (this->_headerMap.count("Host")) {
        return this->_headerMap.at("Host");
    } else {
        return "";
    }
}

std::string Request::get_method(void) const {
    std::istringstream stream(_header);
    std::string method;
    stream >> method;
    return method.c_str();
}

std::string Request::get_user_agent(void) const {
    if (this->_headerMap.count("User-Agent")) {
        return this->_headerMap.at("User-Agent");
    } else {
        return "";
    }
}

std::string Request::get_http_protocol(void) const {
    std::istringstream stream(this->_header);
    std::string method, url, protocol;
    stream >> method >> url >> protocol;
    return protocol.c_str();
}

/***** parsing *****/
void        Request::body_type() {
    size_t contlen_pos = this->_header.find("Content-Length: ");
    if (contlen_pos != std::string::npos) {
        size_t endl = this->_header.find("\r\n", contlen_pos);
        if (endl != std::string::npos) {
            std::string contlen_str = this->_header.substr(contlen_pos + 16, endl - contlen_pos - 16);
            int contlen = std::stoi(contlen_str);
            if (contlen > 0)
                this->_content_len = contlen;
        }
    }

    size_t transenco_pos = this->_header.find("Transfer-Encoding: ");
    if (transenco_pos != std::string::npos) {
        size_t endl = this->_header.find("\r\n", transenco_pos);
        if (endl != std::string::npos) {
            std::string transenco = this->_header.substr(transenco_pos + 19, endl - transenco_pos - 19);
            if (transenco == "chunked")
                this->_chunked = true;
        }
    }

    size_t bound_pos = this->_header.find("boundary=");
    if (bound_pos != std::string::npos) {
        size_t bound_end = this->_header.find("\r\n", bound_pos);
        if (bound_end != std::string::npos) {
            std::string bound_val = this->_header.substr(bound_pos + 9, bound_end - bound_pos - 9);
            if (bound_val.front() == '"' && bound_val.back() == '"')
                bound_val = bound_val.substr(1, bound_val.size() - 2);
            this->_boundary = bound_val;
        }
    }
}

void        Request::set_header(const std::string& header) {
    std::istringstream  iss(header);
    std::string         first_line;
    std::string         line;

    std::getline(iss, first_line, '\n');
    std::istringstream first_line_stream(first_line);
    first_line_stream >> this->_method >> this->_uri;

    while (std::getline(iss, line, '\n')) {
        size_t pos = line.find(':');
        
        if (pos != std::string::npos) {
            std::string field = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            if (field == "Host") {
                size_t port_pos = value.find(":");
                if (port_pos != std::string::npos) {
                    std::string port_str = value.substr(port_pos + 1);
                    this->_port = std::stoi(port_str);
                }
            }
            if (field == "Accept") {
                this->_content_type = value;
            } else if (field == "Content-Length") {
                this->_content_len = std::stol(value);
            }
        }
    }
    body_type();
    this->_header = header;
}

void        Request::add_chunk(std::string chunk) {this->_chunks.push_back(chunk);}

void        Request::add_part(Part* part) {_parts.push_back(part);}

void        Request::parseHeader(void) {
    std::istringstream stream(this->_header);
    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        size_t delimiter = line.find(": ");
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 2);
            this->_headerMap[key] = value;
        }
    }
}

/***** utils *****/
int         Request::has_body() const {
    if (this->chunked())
        return 1;
    else if (!this->_boundary.empty())
        return 2;
    else if (this->_content_len > 0)
        return 3;
    return 0;
}

bool        Request::chunked() const {return this->_chunked;}

void        Request::print_infos() {

    std::cout << "\033[35mREQUEST INFOS:" << std::endl;
    std::cout << "Method: " << this->_method << std::endl;
    std::cout << "URI: " << this->_uri << std::endl;
    std::cout << "Port: " << this->_port << std::endl;
    std::cout << "Accept: " << this->_content_type << std::endl;
    std::cout << "Chunked: " << this->_chunked << std::endl;
    std::cout << "Multipart: " << this->_boundary << std::endl;
    std::cout << "Content-Length: " << this->_content_len << std::endl << std::endl << "\033[0m";
}
