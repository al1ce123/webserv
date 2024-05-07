#include "../../include/server/Core.h"

std::string get_fileContent(const std::string& filename) {
    std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
    if (in) {
        std::ostringstream content;
        content << in.rdbuf();
        in.close();
        return content.str();
    }
    return "";
}

std::string get_contentType(const std::string& filename) {
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

// Function to execute CGI script and capture its output
std::string execute_cgi(const std::string& script_name) {
    std::string command = CGI_PATH + script_name;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "Error executing CGI script";
    }

    char buffer[4096];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    return result;
}

// Set socket to non-blocking mode (apparently epoll needs it)
int set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}
