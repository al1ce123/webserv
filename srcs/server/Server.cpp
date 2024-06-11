/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 17:35:18 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 16:27:23 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"

Server::Server() : _locations() {}

Server::~Server() {}

Server::Server(const Server &src) 
    : _methods(src._methods),
      _sockets(src._sockets),
      _server_name(src._server_name),
      _max_body_size(src._max_body_size),
      _index(src._index),
      _root(src._root),
      _error_pages(src._error_pages),
      _locations(),
      _autoindex(false)
{

    for (size_t i = 0; i < src._locations.size(); ++i) {
        _locations.push_back(new Location(*src._locations[i]));
    }
}

Server& Server::operator=(const Server &rhs) {
    if (this != &rhs) {
        _methods = rhs._methods;
        _sockets = rhs._sockets;
        _server_name = rhs._server_name;
        _max_body_size = rhs._max_body_size;
        _index = rhs._index;
        _root = rhs._root;
        _error_pages = rhs._error_pages;

        for (size_t i = 0; i < _locations.size(); ++i) {
            delete _locations[i];
        }
        _locations.clear();

        for (size_t i = 0; i < rhs._locations.size(); ++i) {
            _locations.push_back(new Location(*rhs._locations[i]));
        }
    }
    return *this;
}

/***** getters *****/
Location* Server::getLocation(size_t index) const {
    return this->_locations[index];
}

size_t Server::getLocationsSize() const {
    return this->_locations.size();
}

std::string Server::getMethod(size_t index) const {
    return this->_methods[index];
}

size_t Server::getMethodsSize(void) const {
    return this->_methods.size();
}

size_t Server::getSocketsSize(void) const {
    return this->_sockets.size();
}

double Server::getMaxBodySize(void) const {
    return this->_max_body_size;
}

const std::map<std::string, std::vector<std::string>>& Server::getSockets(void) const {    
    return this->_sockets;
}

void Server::setAutoindexToTrue(void) {
    this->_autoindex = true;
}

const std::vector<std::string>& Server::getServerName(void) const {
    return this->_server_name;
}

/***** member functions *****/
void Server::printInfos(void) const {
    std::cout << "Methods: ";
    for (size_t i = 0; i < _methods.size(); ++i) {
        std::cout << _methods[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Sockets:" << std::endl;
    for (std::map<std::string, std::vector<std::string> >::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
        std::cout << "  IP: " << it->first << ", Ports: ";
        for (size_t i = 0; i < it->second.size(); ++i) {
            std::cout << it->second[i] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Server Name: ";
    for (size_t i = 0; i < _server_name.size(); ++i) {
        std::cout << _server_name[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Max Body Size: " << _max_body_size << std::endl;

    std::cout << "Index: ";
    for (size_t i = 0; i < _index.size(); ++i) {
        std::cout << _index[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Root: " << _root << std::endl;

    std::cout << "Error Pages:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }

    std::cout << "Locations:\n";
    for (size_t i = 0; i < _locations.size(); ++i) {
        _locations[i]->printInfos();
    }

    std::cout << "Autoindex: " << (_autoindex ? "true" : "false") << std::endl;

    std::cout << "CGI Extensions: ";
    for (size_t i = 0; i < _cgi_ext.size(); ++i) {
        std::cout << _cgi_ext[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Return Path: " << _ret_path << std::endl;
    std::cout << "CGI Path: " << _cgi_path << std::endl;
}

void Server::parser(std::ifstream& file) {

    std::string line;

    while (std::getline(file, line)) {

        if (line.empty()) {
            continue;
        }
        
        if (line == "}") {
            break;
        }

        size_t pos;

        pos = line.find("listen");
        if (pos != std::string::npos) {
            parseListenLine(line);
            continue;
        }

        pos = line.find("server_name");
        if (pos != std::string::npos) {
            parseServerNameLine(line);
            continue;
        }

        pos = line.find("root");
        if (pos != std::string::npos) {
            parseRootLine(line);
            continue;
        }

        pos = line.find("index");
        if (pos != std::string::npos) {
            parseIndexLine(line);
            continue;
        }

        pos = line.find("location");
        if (pos != std::string::npos) {
            parseLocationLine(file, line);
            continue;
        }

        pos = line.find("error_page");
        if (pos != std::string::npos) {
            parseErrorPageLine(line);
            continue;
        }

        pos = line.find("client_max_body_size");
        if (pos != std::string::npos) {
            parseClientMaxBodySizeLine(line);
            continue;
        }

    }
}

std::vector<int> Server::getPorts(void) {
    std::vector<int> result;
    int temp;

    for (std::map<std::string, std::vector<std::string> >::iterator it = this->_sockets.begin(); it != this->_sockets.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            std::stringstream ss(it->second[i]);
            ss >> temp;
            result.push_back(temp);
        }
    }
    return result; 
}

int Server::parseListenLine(std::string line) {

    std::string ip;
    std::string saved_ip;
    std::string port;
    std::string nbr;
    size_t pointPos;
    const char* charPtr;
    size_t pos;
    std::string subStr;
    int intValue;

    subStr = line.substr(0, 11);

    if (subStr != "    listen ") {
        std::cerr << "Syntax Error" << '\n';
        return 1;
    }

    subStr = line.substr(11);
    

    // IP + PORT CASE
    pos = subStr.find(":");
    if (pos != std::string::npos) {

        // Check ip address format
        ip = subStr.substr(0, pos);
        saved_ip = ip;

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (size_t i = 0; i < nbr.length(); ++i) {
                if (!isdigit(nbr[i])) {
                    std::cerr << "[IP] Syntax Error" << '\n';
                    return 1;
                }
            }
            charPtr = nbr.c_str();
            intValue = atoi(charPtr);
            if (intValue < 0 || intValue > 255) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }

        ip = ip.substr(pointPos + 1);

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (size_t i = 0; i < nbr.length(); ++i) {
                if (!isdigit(nbr[i])) {
                    std::cerr << "[IP] Syntax Error" << '\n';
                    return 1;
                }
            }
            charPtr = nbr.c_str();
            int intValue = atoi(charPtr);
            if (intValue < 0 || intValue > 255) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }

        ip = ip.substr(pointPos + 1);

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (size_t i = 0; i < nbr.length(); ++i) {
                if (!isdigit(nbr[i])) {
                    std::cerr << "[IP] Syntax Error" << '\n';
                    return 1;
                }
            }
            charPtr = nbr.c_str();
            int intValue = atoi(charPtr);
            if (intValue < 0 || intValue > 255) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }

        ip = ip.substr(pointPos + 1);

        nbr = ip.substr(0);
        // Check if each characters are digits
        for (size_t i = 0; i < nbr.length(); ++i) {
            if (!isdigit(nbr[i])) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }
        charPtr = nbr.c_str();
        int intValue = atoi(charPtr);
        if (intValue < 0 || intValue > 255) {
            std::cerr << "[IP] Syntax Error" << '\n';
            return 1;
        }

        // Check port number format
        port = subStr.substr(pos + 1);
        // Check if each characters are digits
        for (size_t i = 0; i < port.length(); ++i) {
            if (!isdigit(port[i])) {
                std::cerr << "[PORT] Syntax Error" << '\n';
                return 1;
            }
        }
        charPtr = port.c_str();
        intValue = atoi(charPtr);
        if (intValue < 0 || intValue > 65535) {
            std::cerr << "[PORT] Syntax Error" << '\n';
            return 1;
        }

        this->_sockets[saved_ip].push_back(port);
        return 0;
    }

    // ONLY IP CASE
    pos = subStr.find(".");
    if (pos != std::string::npos) {
        // Check ip address format
        ip = subStr;
        saved_ip = ip;

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (size_t i = 0; i < nbr.length(); ++i) {
                if (!isdigit(nbr[i])) {
                    std::cerr << "[IP] Syntax Error" << '\n';
                    return 1;
                }
            }
            charPtr = nbr.c_str();
            intValue = atoi(charPtr);
            if (intValue < 0 || intValue > 255) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }

        ip = ip.substr(pointPos + 1);

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (size_t i = 0; i < nbr.length(); ++i) {
                if (!isdigit(nbr[i])) {
                    std::cerr << "[IP] Syntax Error" << '\n';
                    return 1;
                }
            }
            charPtr = nbr.c_str();
            int intValue = atoi(charPtr);
            if (intValue < 0 || intValue > 255) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }

        ip = ip.substr(pointPos + 1);

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (size_t i = 0; i < nbr.length(); ++i) {
                if (!isdigit(nbr[i])) {
                    std::cerr << "[IP] Syntax Error" << '\n';
                    return 1;
                }
            }
            charPtr = nbr.c_str();
            int intValue = atoi(charPtr);
            if (intValue < 0 || intValue > 255) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }

        ip = ip.substr(pointPos + 1);

        nbr = ip.substr(0);
        // Check if each characters are digits
        for (size_t i = 0; i < nbr.length(); ++i) {
            if (!isdigit(nbr[i])) {
                std::cerr << "[IP] Syntax Error" << '\n';
                return 1;
            }
        }
        charPtr = nbr.c_str();
        int intValue = atoi(charPtr);
        if (intValue < 0 || intValue > 255) {
            std::cerr << "[IP] Syntax Error" << '\n';
            return 1;
        }

        port = "80"; // or 8080 non-sudo user

        this->_sockets[saved_ip].push_back(port);
        return 0;
    }

    // ONLY PORT CASE
    saved_ip = "0.0.0.0";
    port = subStr.substr(0, subStr.length() - 1);

    // Check if each characters are digits
    for (size_t i = 0; i < port.length(); ++i) {
        if (!isdigit(port[i])) {
            std::cerr << " Only port [PORT] Syntax Error" << '\n';
            return 1;
        }
    }
    charPtr = port.c_str();
    intValue = atoi(charPtr);
    if (intValue < 0 || intValue > 65535) {
        std::cerr << "[PORT] Syntax Error" << '\n';
        return 1;
    }

    this->_sockets[saved_ip].push_back(port);

    return 0;
}

std::string Server::parseRootLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 4);

    result = line.substr(pos + 1, line.length() - (pos + 1) - 1);
    this->_root = result;
    return result;
}

void Server::parseServerNameLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 4);
    size_t endPos = line.find(" ", pos + 1);
    if (endPos == std::string::npos) {
        result = line.substr(pos + 1, line.length() - (pos + 1) - 1);
        this->_server_name.push_back(result);
        return;
    } else {
        while (pos != std::string::npos && endPos != std::string::npos) {
            result = line.substr(pos + 1, endPos - (pos + 1));
            this->_server_name.push_back(result);
            pos = endPos;
            endPos = line.find(" ", pos + 1);
        }
        result = line.substr(pos + 1, line.length() - (pos + 1) -1);
        this->_server_name.push_back(result);
    }
}

void Server::parseLocationLine(std::ifstream& file, std::string line) {
    try {
        Location* ptr = new Location();

        ptr->parseRouteLine(line);

        std::string directive;
        std::string newLine;

        while (true) {
            std::getline(file, newLine);

            if (newLine == "    }") {
                break;
            }

            directive = ptr->getDirective(newLine);

            if (directive == "root") {
                ptr->parseRootLine(newLine);
            } else if (directive == "index") {
                ptr->parseIndexLine(newLine);
            } else if (directive == "limit_except") {
                ptr->parseAllowMethodsLine(newLine);
            } else if (directive == "return") {
                // ptr->getRedir(newLine);
                continue;
            } else if (directive == "autoindex") {
                ptr->setAutoindexToTrue();
            } else if (directive == "client_max_body_size") {
                ptr->parseClientMaxBodySize(newLine);
            } else {
                continue;
            }
        }
        
        this->_locations.push_back(ptr);

    } catch (std::bad_alloc& ba) {
        std::cerr << "Bad allocation: " << ba.what() << '\n';
    }
}

void Server::parseIndexLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 4);
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

void Server::parseErrorPageLine(std::string line) {

    std::string status_code;
    std::string file_name;

    size_t pos = line.find(" ", 4);
    size_t endPos = line.find(" ", pos + 1);
    status_code = line.substr(pos + 1, endPos - (pos + 1));

    pos = endPos + 1;
    file_name = line.substr(pos, line.length() - pos - 1);
    this->_error_pages.insert(std::make_pair(status_code, file_name));
}

void Server::parseClientMaxBodySizeLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 4);
    size_t endPos = line.find("M", pos + 1);
    result = line.substr(pos + 1, endPos - (pos + 1));

    std::stringstream ss(result);
    double size;
    ss >> size;

    this->_max_body_size = size;
}

void Server::parseAllowMethodsLine(std::string line) {
    
    std::string temp;
    size_t pos;
    size_t endPos;

    pos = line.find(" ", 4);
    endPos = line.find(" ", pos + 1);
    while (endPos != std::string::npos) {
        temp = line.substr(pos + 1, endPos - (pos + 1));
        this->_methods.push_back(temp);
        pos = endPos;
        endPos = line.find(" ", pos + 1);
    }
    temp = line.substr(pos + 1, line.length() - (pos + 2));
    this->_methods.push_back(temp);
}

bool Server::hasMethods(void) const {
    if (this->_methods.empty()) {
        return false;
    }
    return true;
}

std::string Server::getErrorPage(std::string status, std::string actual) {
    if (!this->_error_pages.empty()) {
        for (std::map<std::string, std::string>::const_iterator it = this->_error_pages.begin(); it != this->_error_pages.end(); it++) {
            if (it->first.find(status) != std::string::npos) {
                return it->second;
            }
        }
    }
    return actual;
}