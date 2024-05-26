#include "../../include/server/Location.h"

// Constructors
Location::Location() : _autoindex(false) {}

Location::Location(const Location &src) {}

Location::~Location() {}

Location& Location::operator=(const Location &rhs) {}

// Member functions
std::string Location::getTheRoute(std::string line) {

        std::string result;

        size_t pos = line.find("/");
        size_t endPos = line.find_last_of(" ");
        if (pos != std::string::npos) {
            result = line.substr(pos, endPos - pos);
        }
        this->_route = result;
        return result;
}

std::string Location::getDirective(std::string line) {

    std::string result;
    size_t startIndex = 8;

    size_t endPos = line.find(" ", startIndex);
    result = line.substr(8, endPos - 8);
    return result;
}

std::string Location::getRoot(std::string line) {

    std::string result;

    size_t pos = line.find(" ", 8);

    result = line.substr(pos + 1, line.length() - (pos + 1) - 1);
    this->_root = result;
    return result;
}

void Location::getIndex(std::string line) {

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

void Location::getMethods(std::string line) {

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

std::string Location::getRedir(std::string line) {
    // need to send a new http request to the new domain => changing the host header I guess
}

std::string Location::setAutoindexToTrue(std::string line) {
    this->_autoindex = true;
}

std::string Location::getMaxClientBodySize(std::string line) {
    
}