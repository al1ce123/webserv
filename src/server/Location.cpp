#include "../../include/server/Location.h"

Location::Location(std::string route) {
    this->_route = route;
}

Location::Location(const Location &src) {}

Location::~Location() {}

Location    &Location::operator=(const Location &rhs) {}