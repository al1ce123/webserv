/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 17:28:08 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/07 11:16:39 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Core.hpp"

std::string get_fileContent(const std::string& filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::ostringstream content;
        content << in.rdbuf();
        in.close();
        return content.str();
    }
    return "";
}

std::string content_type(const std::string& filename) {
    if (filename.find(".html") != std::string::npos) {
        return "text/html";
    } else if (filename.find(".css") != std::string::npos) {
        return "text/css";
    } else if (filename.find(".js") != std::string::npos) {
        return "application/javascript";
    } else {
        return "application/octet-stream"; // Default to binary data if content type is unknown
    }
}

std::string exec_cgi(const std::string& script_name, int timeout_seconds) {
    std::string command = CGI_PATH + script_name;
    FILE* pipe = popen(command.c_str(), "r");
    
    if (!pipe)
        return "Error executing CGI script";

    // Set the pipe file descriptor to non-blocking mode
    int fd = fileno(pipe);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    char buffer[1024];
    std::string result;
    time_t start_time = time(NULL);
    bool timed_out = false;
    bool done = false;

    while (!done) {
        // Attempt to read from the pipe
        if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        } else {
            // Check if the read operation would block
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // Check for timeout
                time_t current_time = time(NULL);
                double elapsed_seconds = difftime(current_time, start_time);
                if (elapsed_seconds >= timeout_seconds) {
                    timed_out = true;
                    break;
                }
                // Sleep for a short duration to avoid busy-waiting
                usleep(10000); // 10 milliseconds
            } else {
                // Other error occurred
                break;
            }
        }

        // Check for end of file
        if (feof(pipe)) {
            done = true;
        }
    }

    // Close the pipe
    pclose(pipe);

    if (timed_out)
        return "Timeout while reading CGI output";
    return result;
}

std::string read_line(int client_socket, Response *res) {
    std::string line;
    ssize_t     bytes;
    char        c;

    while (true) {
        bytes = recv(client_socket, &c, 1, 0);
        if (bytes < 0) {
            res->set_status("500", " Internal Server Error");
            throw Core::InterServError();
        } else if (bytes == 0 || c == '\n')
            break;
        else if (c != '\r')
            line += c;
    }
    return line;
}

size_t      max_body_size(const Request *req, Server *serv) {
    size_t ret = 0;
    std::string path = req->get_uri();

    while (true) {
        size_t  pos = path.find("/", 1);
        if (pos == std::string::npos)
            break;
        std::string tmp = path.substr(0, pos);
        path = path.substr(tmp.length(), (path.length() - tmp.length()));

        for (size_t i = 0; i < serv->getLocationsSize(); i++) {
            if (tmp.find(serv->getLocation(i)->getRoute()) != std::string::npos) {
                if (serv->getLocation(i)->getMaxBodySize() > 0)
                    ret = serv->getLocation(i)->getMaxBodySize();
            }
        }
    }
    return ret;
}