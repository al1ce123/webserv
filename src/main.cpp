#include "../include/server/Core.h"
#include "../include/server/Server.h"
#include "../include/server/Location.h"


/* ---- TO MAKE THE CODE WORK YOU NEED TO CHANGE
        THIS VARIABLE <path_to_webserv_from_home>
        IN THE <void Core::generate_routes(void)> function ---- */


int webserv(Core *core)
{
    core->bind_ports();
    core->generate_routes();

    // Create specific file descriptor for epoll
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        std::cerr << "Failed to create epoll file descriptor\n";
        return 1;
    }

    struct epoll_event event, events[MAX_EVENTS];

    event.events = EPOLLIN;
    // Add every server socket previously created to epoll instance
    for (int i = 0; i < core->get_server_sockets_size(); ++i) {
        event.data.fd = core->get_server_socket(i);
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, core->get_server_socket(i), &event) == -1) {
            std::cerr << "Failed to add file descriptor to epoll\n";
            return 1;
        }
    }

    // Main loop
    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, 2000); // nfds is the number of events detected by epoll_wait
        if (nfds == -1) {
            std::cerr << "epoll_wait failed\n";
            return 1;
        }

        int client_sockfd;
        int i, j;

        // Handle server sockets
        for (i = 0; i < nfds; i++) {
            if ((events[i].events & EPOLLIN) != 0) {
                for (j = 0; j < core->get_server_sockets_size(); ++j) {
                    if (events[i].data.fd == core->get_server_socket(j)) {
                        client_sockfd = accept(events[i].data.fd, NULL, NULL);
                        if (client_sockfd == -1) {
                            std::cerr << "Failed to accept connection\n";
                            continue;
                        }

                        core->map_sock(client_sockfd, core->get_psock(events[i].data.fd)); // ?

                        if (set_non_blocking(client_sockfd) == -1) {
                            close(client_sockfd);
                            continue;
                        }

                        event.events = EPOLLIN;
                        event.data.fd = client_sockfd;

                        // Add client socket to epoll
                        if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &event) == -1) {
                            std::cerr << "Failed to add file descriptor to epoll\n";
                            close (client_sockfd);
                            continue;
                        }

                        // Adding client socket to _client_sockets vector
                        core->add_client_socket_to_vector(client_sockfd);
                    }
                }
            }
        }

        // Handle client sockets
        for (i = 0; i < nfds; i++) {
            if ((events[i].events & EPOLLIN) != 0) {
                char buffer[MAX_REQUEST_SIZE] = {0};
                int bytes_received = recv(events[i].data.fd, buffer, sizeof(buffer), 0); // Need to handle partial reads by checking double CRLF \r\n\r\n and looping on recv()
                if (bytes_received <= 0) {
                    core->unmap_sock(events[i].data.fd); // Unmap the client socket from psock map
                    close(events[i].data.fd);

                    // Remove client socket from epoll
                    // if (epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1) {
                    //     std::cerr << "Failed to remove client file descriptor from epoll\n";
                    // }

                } else {
                        // std::cout << bytes_received << '\n';
                    std::string request(buffer);


                    std::cout << request << '\n';


                    // Parse the URL to retrieve the file
                    // using a simple member function for testing, this will be done wilt the HTTPRequest class
                    // need an POST request parser to make cgi work
                    std::string reqfile;
                    reqfile = core->get_file_name(request);
                    if (reqfile.empty()) {
                        reqfile = "index.html";
                    }

                    // Display requested file for debugging
                    std::cout << reqfile << '\n';

                    std::stringstream httpResponse;
                    std::string httpResponseBody;

                    if (!reqfile.empty()) {
                        // Check if the requested file is a CGI script
                        size_t dotPos = reqfile.find_last_of(".");
                        if (dotPos != std::string::npos) {
                            std::string extension = reqfile.substr(dotPos);
                            if (extension == ".cgi") {
                                // Handle CGI script request
                                std::string cgi_response = execute_cgi(reqfile);
                                if (!cgi_response.empty()) {
                                    // Send CGI response to client

                                    // Display the reaponse for debugging
                                    std::cout << "CGI RESPONSE:\n" << cgi_response << '\n';

                                    send(events[i].data.fd, cgi_response.c_str(), cgi_response.length(), 0);
                                    continue;
                                }
                                else {
                                    httpResponse << "HTTP/1.1 404 Not Found\r\n";
                                    httpResponse << "Content-Type: " << get_contentType(core->get_file_path("404.html") + "404.html") << "\r\n";
                                    httpResponseBody = get_fileContent(core->get_file_path("404.html") + "404.html");
                                    httpResponse << "Content-Length: " << httpResponseBody.size() << "\r\n\r\n";
                                    break;
                                }
                            }
                        }  

                        std::string file_path;
                        if (reqfile == "index.html") {
                            file_path = "/home/nlence-l/coding/webserv/www/index.html";
                        } else {
                            file_path = core->get_file_path(reqfile) + reqfile;
                        }

                        // Display file path for debugging
                        std::cout << file_path << '\n';

                        httpResponseBody = get_fileContent(file_path);
                        if (!httpResponseBody.empty()) {
                            httpResponse << "HTTP/1.1 200 OK" << "\r\n";
                            httpResponse << "Content-Type: " << get_contentType(file_path) << "\r\n";
                            httpResponse << "Content-Length: " << httpResponseBody.size() << "\r\n\r\n";
                        } else {
                            httpResponse << "HTTP/1.1 404 Not Found\r\n";
                            httpResponse << "Content-Type: " << get_contentType(core->get_file_path("404.html") + "404.html") << "\r\n";
                            httpResponseBody = get_fileContent(core->get_file_path("404.html") + "404.html");
                            httpResponse << "Content-Length: " << httpResponseBody.size() << "\r\n\r\n";
                        }
                    } else {
                        httpResponse << "HTTP/1.1 400 Bad Request\r\n";
                        httpResponse << "Content-Type: " << get_contentType(core->get_file_path("400.html") + "400.html") << "\r\n";
                        httpResponseBody = get_fileContent(core->get_file_path("400.html") + "400.html");
                        httpResponse << "Content-Lenght: " << httpResponseBody.size() << "\r\n\r\n";
                    }

                    std::stringstream ss;
                    ss << httpResponse.str();
                    ss << httpResponseBody;

                    std::string result = ss.str();

                    // Display result for debugging
                    std::cout << result << '\n';

                    // Send HTTPResponse
                    send(events[i].data.fd, result.c_str(), result.length(), 0);
                }
            }
        }
    }
    core->close_sock();
    close(epfd);
    return 0;
}

int main(int ac, char **av)
{
    (void)av;
    (void)ac;

    try {
        Core *core = new Core;
        core->add_port(8084);
        // serv->add_port(8001);
        // serv->add_port(8002);
        webserv(core);
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}