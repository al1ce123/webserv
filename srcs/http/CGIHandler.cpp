#include "../../include/http/CGIHandler.hpp"
#include "../../include/http/Request.hpp"

#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <stdlib.h>

class Request;

// Constructors
CGIHandler::CGIHandler() {}

CGIHandler::CGIHandler(CGIHandler& src)
    : _env(src._env), _path(src._path) {}

CGIHandler::~CGIHandler() {}

CGIHandler& CGIHandler::operator=(const CGIHandler& rhs) {
    if (this != &rhs) {
        _env = rhs._env;
        _path = rhs._path;
    }
    return *this;
}

// Member functions
void CGIHandler::initEnv(Request& req) {

    // setenv("AUTH_TYPE", "", 1);
    setenv("CONTENT_LENGTH", req.get_content_length().c_str(), 1); 
    setenv("CONTENT_TYPE", req.get_content_type().c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("QUERY_STRING", req.get_query_string().c_str(), 1);
    setenv("REMOTE_HOST", req.get_host().c_str(), 1);
    // setenv("REMOTE_IDENT", "", 1);
    // setenv("REMOTE_USER", "", 1);
    setenv("REQUEST_METHOD", req.get_method().c_str(), 1);
    setenv("SERVER_PROTOCOL", req.get_http_protocol().c_str(), 1);
    setenv("SERVER_SOFTWARE", "webserv/1.1", 1);
    setenv("HTTP_USER_AGENT", req.get_user_agent().c_str(), 1);
    // setenv("HTTP_ACCEPT", "*/*", 1);
    // setenv("REMOTE_ADDR", req.getIpAddrCgi(), 1);
    // setenv("SCRIPT_NAME", req.getScriptNameCgi(), 1);
    // setenv("SERVER_NAME", req.getServerNameCgi(), 1);
    // setenv("SERVER_PORT", req.getPortCgi(), 1);

}

void CGIHandler::execCgiScript(std::string& script_path, Request& req /*int client_socket*/) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "pipe(): error detected" << '\n';
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "fork(): error detected" << '\n';
        return;
    }

    if (pid == 0) {
        this->initEnv(req);

        close(pipefd[0]);

        std::map<std::string, std::string>::iterator it;
        std::string result;
        std::vector<char*> args;
        std::vector<char*> env;

        args.push_back(strdup(script_path.c_str()));
        args.push_back(nullptr);

        for (it = this->_env.begin(); it != _env.end(); it++) {
            result = it->first + '=' + it->second;
            env.push_back(strdup(result.c_str()));
        }
        env.push_back(nullptr);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execve(script_path.c_str(), &args[0], &env[0]);
    } else {
        close(pipefd[1]);

        int status;
        waitpid(pid, &status, 0);

        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer)) > 0)) {
            std::cout.write(buffer, bytes_read);
            // Send the data to the client instead of stdout ???
        }
        close(pipefd[0]);
    }
}

