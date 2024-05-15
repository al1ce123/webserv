#include "../../include/server/Server.h"

Server::Server() {}

Server::Server(const Server &src) {}

Server::~Server() {}

Server& Server::operator=(const Server &rhs) {}

void Server::parser(std::string file)
{
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

        // Search for listen directive
        pos = line.find("listen");
        if (pos != std::string::npos) {
            parse_listen_line(line);
            continue;
        }
        
        // Search for server_name directive
        pos = line.find("server_name");
        if (pos != std::string::npos) {
            parse_server_name_line(line);
            continue;
        }

        // Search for server root directive
        pos = line.find("root");
        if (pos != std::string::npos) {
            parse_root_line(line);
            continue;
        }

        // Search for index directive
        pos = line.find("index");
        if (pos != std::string::npos) {
            parse_index_line(line);
            continue;
        }

        // Search for location directive
        pos = line.find("location");
        if (pos != std::string::npos) {
            parse_location_line(configFile, line);
            continue;
        }

        // Search for error_page directive
        pos = line.find("error_page");
        if (pos != std::string::npos) {
            parse_error_page_line(line);
            continue;
        }
    }

    configFile.close();
}

Location* Server::parse_location_line(std::ifstream& file, std::string route)
{
    try {
        Location* ptr = new Location(route);
        //
    } catch (std::bad_alloc& ba){
        std::cerr << "Bad allocation: " << ba.what() << '\n';
    }

    // parse location block from '{' to '}'
    std::string line;

    while (line[0] != '}') {
        std::getline(file, line);
        std::string directive;

        // retrieve directive

        if (directive == "root") {
            ptr->get_root();
        } else if (directive == "index") {
            ptr->get_index();
        } else if (directive == "methods") {
            ptr->get_methods();
        } else if (directive == "redir") {
            ptr->get_redir();
        } else if (directive == "autoindex") {
            ptr->get_autoindex();
        }
    }
}


int Server::parse_listen_line(std::string line)
{
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

