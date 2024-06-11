/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 21:18:59 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/09 15:49:01 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Location.hpp"

Location::Location() : _autoindex(false) {}

Location::Location(const Location &src)
    : _max_body_size(src._max_body_size),
      _route(src._route),
      _root(src._root),
      _index(src._index),
      _methods(src._methods),
      _redir(src._redir),
      _autoindex(src._autoindex),
      _error_pages(src._error_pages)
{}

Location::~Location() {}

Location& Location::operator=(const Location &rhs) {
    if (this != &rhs) {
        _max_body_size = rhs._max_body_size;
        _route = rhs._route;
        _root = rhs._root;
        _index = rhs._index;
        _methods = rhs._methods;
        _redir = rhs._redir;
        _autoindex = rhs._autoindex;
        _error_pages = rhs._error_pages;
    }
    return *this;
}

/***** testing *****/
void Location::printInfos() const {
    std::cout << "      Max Body Size: " << getMaxBodySize() << std::endl;
    std::cout << "      Route: " << getRoute() << std::endl;
    std::cout << "      Root: " << getRoot() << std::endl;

    std::cout << "      Index: ";
    std::vector<std::string> index = getIndex();
    for (size_t i = 0; i < index.size(); ++i) {
        std::cout << index[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "      Methods: ";
    std::vector<std::string> methods = getMethod();
    for (size_t i = 0; i < getMethodsSize(); ++i) {
        std::cout << methods[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "      Redirections:" << std::endl;
    std::map<std::string, std::string> redir = _redir;
    for (std::map<std::string, std::string>::const_iterator it = redir.begin(); it != redir.end(); ++it) {
        std::cout << "  From: " << it->first << ", To: " << it->second << std::endl;
    }

    std::cout << "      Autoindex: " << (_autoindex ? "true" : "false") << std::endl;

    std::cout << "      Error Pages:" << std::endl;
    std::map<std::string, std::string> errorPages = getErrorPages();
    for (std::map<std::string, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }

    std::cout << "      CGI Extensions: ";
    std::vector<std::string> cgiExt = getCgiExt();
    for (size_t i = 0; i < cgiExt.size(); ++i) {
        std::cout << cgiExt[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "      Return Path: " << getRetPath() << std::endl;
    std::cout << "      CGI Path: " << getCgiPath() << std::endl;
    std::cout << "\n";
}

/***** getters *****/
// std::string Location::getRedir(std::string line) {
//     // need to send a new http request to the new domain => changing the host header I guess
// }

std::string Location::getDirective(std::string line) {

    std::string result;
    size_t startIndex = 8;

    if (line.length() < 8)
        return "";
    size_t endPos = line.find(" ", startIndex);
    result = line.substr(8, endPos - 8);
    return result;
}

std::string Location::getRoute(void) const {
    return this->_route;
}

std::string Location::getMethod(size_t index) {
    return this->_methods[index];
}

size_t Location::getMethodsSize(void) const {
    return this->_methods.size();
}

double Location::getMaxBodySize(void) const {
    return this->_max_body_size;
}

std::string                         Location::getRoot(void) const {
    return this->_root;
}

std::vector<std::string>            Location::getIndex(void) const {
    return this->_index;
}

std::vector<std::string>            Location::getCgiExt(void) const {
    return this->_cgi_ext;
}

std::string                         Location::getRetPath(void) const {
    return this->_ret_path;
}

std::string                         Location::getCgiPath(void) const {
    return this->_cgi_path;
}

std::map<std::string, std::string>            Location::getErrorPages(void) const {
    return this->_error_pages;
}

std::vector<std::string>            Location::getMethod(void) const {
    return this->_methods;
}

/***** setters *****/
void Location::setAutoindexToTrue(void) {
    this->_autoindex = true;
}

/***** parsing *****/
void Location::parseRouteLine(std::string line) {

        std::string result;

        size_t pos = line.find("/");
        size_t endPos = line.find_last_of(" ");
        if (pos != std::string::npos) {
            result = line.substr(pos, endPos - pos);
        }
        this->_route = result;
}

void Location::parseRootLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 8);

    result = line.substr(pos + 1, line.length() - (pos + 1) - 1);
    this->_root = result;
}

void Location::parseIndexLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 8);
    size_t endPos = line.find(" ", pos + 1);

    if (endPos == std::string::npos) {
        result = line.substr(pos + 1, line.length() - (pos + 1) - 1);
        this->_index.push_back(result);
        return;
    } else {
        while (pos != std::string::npos && endPos != std::string::npos) {
            result = line.substr(pos + 1, endPos - (pos + 1));
            this->_index.push_back(result);
            pos = endPos;
            endPos = line.find(" ", pos + 1);
        }
        result = line.substr(pos + 1, line.length() - (pos + 1) -1);
        this->_index.push_back(result);
    }
}

void Location::parseAllowMethodsLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 8);
    size_t endPos = line.find(" ", pos + 1);
    if (endPos == std::string::npos) {
        result = line.substr(pos + 1, line.length() - (pos + 1) - 1);
        this->_methods.push_back(result);
        return;
    } else {
        while (pos != std::string::npos && endPos != std::string::npos) {
            result = line.substr(pos + 1, endPos - (pos + 1));
            this->_methods.push_back(result);
            pos = endPos;
            endPos = line.find(" ", pos + 1);
        }
        result = line.substr(pos + 1, line.length() - (pos + 1) -1);
        this->_methods.push_back(result);
    }

}

void Location::parseClientMaxBodySize(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 8);
    size_t endPos = line.find("M", pos + 1);
    result = line.substr(pos + 1, endPos - (pos + 1));

    std::stringstream ss(result);
    double size;
    ss >> size;

    this->_max_body_size = size;
}

void Location::parseErrorPageLine(std::string line) {

    std::string status_code;
    std::string file_name;

    size_t pos = line.find(" ", 8);
    size_t endPos = line.find(" ", pos + 1);
    status_code = line.substr(pos + 1, endPos - (pos + 1));

    pos = endPos + 1;
    file_name = line.substr(pos, line.length() - pos - 1);
    this->_error_pages.insert(std::make_pair(status_code, file_name));
}

/***** utils *****/
bool Location::hasMethods(void) const {
    if (this->_methods.empty()) {
        return false;
    }
    return true;
}

std::string Location::getErrorPage(std::string status, std::string actual) {
    if (!this->_error_pages.empty()) {
        for (std::map<std::string, std::string>::const_iterator it = this->_error_pages.begin(); it != this->_error_pages.end(); it++) {
            if (it->first.find(status) != std::string::npos) {
                return it->second;
            }
        }
    }
    return actual;
}