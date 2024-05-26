#include "../../include/server/Server.h"

Server::Server() : _locations(NULL), _size(0), _capacity(0) {}

Server::Server(const Server &src) {}

Server::~Server() {}

Server& Server::operator=(const Server &rhs) {}

void Server::parser(std::string file) {

    std::ifstream configFile("file");
    if (!configFile) {
        std::cerr << "Can't open the file" << '\n';
    }

    std::string line;

    while (std::getline(configFile, line)) {
        // Skip new lines
        if (line.empty()) {
            continue;
        }
        
        if (line != "server {") {
            std::cerr << "Syntax Error" << '\n';
        } else {
            continue;
        }

        size_t pos;

        // Look for Server's listen directive
        pos = line.find("listen");
        if (pos != std::string::npos) {
            parseListenLine(line);
            continue;
        }
        
        // Look for Server's server_name directive
        pos = line.find("server_name");
        if (pos != std::string::npos) {
            parseServerNameLine(line);
            continue;
        }

        // Look for Server's root directive
        pos = line.find("root");
        if (pos != std::string::npos) {
            parseRootLine(line);
            continue;
        }

        // Look for Server's index directive
        pos = line.find("index");
        if (pos != std::string::npos) {
            parseIndexLine(line);
            continue;
        }

        // Look for Server's location directive
        pos = line.find("location");
        if (pos != std::string::npos) {
            Location* newLocation;
            newLocation = parseLocationLine(configFile, line);
    
            continue;
        }

        // Look for Server's error_page directive
        pos = line.find("error_page");
        if (pos != std::string::npos) {
            parseErrorPageLine(line);
            continue;
        }

        // Look for Server's client_max_body_size directive
        pos = line.find("client_max_body_size");
        if (pos != std::string::npos) {
            parseClientMaxBodySizeLine(line);
            continue;
        }
    }
    configFile.close();
}

Location* Server::parseLocationLine(std::ifstream& file, std::string line) {

    try {
        Location* ptr = new Location();

        ptr->getTheRoute(line);

        std::string directive;
        std::string newLine;

        while (line[0] != '}') {
            std::getline(file, newLine);

            directive = ptr->getDirective(newLine);

            if (directive == "root") {
                ptr->getRoot(newLine);
            } else if (directive == "index") {
                ptr->getIndex(newLine);
            } else if (directive == "limit_except") {
                ptr->getMethods(newLine);
            } else if (directive == "return") {
                ptr->getRedir(newLine);
            } else if (directive == "autoindex") {
                ptr->setAutoindexToTrue(newLine);
            } else if (directive == "client_max_body_size") {
                ptr->getMaxClientBodySize(newLine);
            } else {
                continue;
            }
        }
    } catch (std::bad_alloc& ba) {
        std::cerr << "Bad allocation: " << ba.what() << '\n';
    }
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
    

    pos = subStr.find(":"); // Ip + port case
    if (pos != std::string::npos) {

        // Check ip address format
        ip = subStr.substr(0, pos);
        saved_ip = ip;

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (int i = 0; i < nbr.length(); ++i) {
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
            for (int i = 0; i < nbr.length(); ++i) {
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
            for (int i = 0; i < nbr.length(); ++i) {
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
        for (int i = 0; i < nbr.length(); ++i) {
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
        for (int i = 0; i < port.length(); ++i) {
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

        // Insert the socket
        this->_sockets.insert(std::make_pair(saved_ip, port));
        return 0;
    }

    pos = subStr.find("."); // Only ip case
    if (pos != std::string::npos) {
        // Check ip address format
        ip = subStr;
        saved_ip = ip;

        pointPos = ip.find(".");
        if (pointPos != std::string::npos) {
            nbr = ip.substr(0, pointPos);
            // Check if each characters are digits
            for (int i = 0; i < nbr.length(); ++i) {
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
            for (int i = 0; i < nbr.length(); ++i) {
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
            for (int i = 0; i < nbr.length(); ++i) {
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
        for (int i = 0; i < nbr.length(); ++i) {
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

        // Insert the socket
        this->_sockets.insert(std::make_pair(saved_ip, port));
        return 0;
    }

    // Only port case
    saved_ip = "0.0.0.0";
    port = subStr;
    // Check if each characters are digits
    for (int i = 0; i < port.length(); ++i) {
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

    this->_sockets.insert(std::make_pair(saved_ip, port));
    return 0;
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

std::string Server::parseRootLine(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 4);

    result = line.substr(pos + 1, line.length() - (pos + 1) - 1);
    this->_root = result;
    return result;
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

void Server::resize(size_t new_capacity) {
    Location** new_locations = new Location* [new_capacity];
    for (size_t i = 0; i < this->_size; i++) {
        new_locations[i] = this->_locations[i];
    }
    delete[] this->_locations;
    this->_locations = new_locations;
    this->_capacity = new_capacity;
}

void Server::addLocation(Location* location) {
    size_t new_capacity;
    if (this->_size == this->_capacity) {
        if (this->_capacity == 0) {
            new_capacity = 1;
        } else {
            new_capacity = this->_capacity * 2;
        }
        resize(new_capacity);
    }
    this->_locations[this->_size++] = location;
}

std::string Server::parseClientMaxBodySizeLine(std::string line) {

}


