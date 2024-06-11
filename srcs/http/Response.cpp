/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 17:35:01 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 16:29:03 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Core.hpp"
#include "../../include/http/Response.hpp"

Response::Response() : _content_len(0) {
    (void)this->_content_len;
}

Response::~Response() {}

/***** getters *****/
std::string Response::get_file() {
    return this->_file;
}

std::string Response::get_status() {
    return this->_status;
}

void        Response::set_head() { 
    std::stringstream head;
    head << "HTTP/1.1 " << this->_status << this->_status_msg << "\r\n";
    head << "Content-Type: " + content_type(this->_file) + "\r\n";
    head << "Content-Length: " + std::to_string(this->_body.size()) + "\r\n";
    head << "\r\n";
    this->_header = head.str();
}

void        Response::set_body(const std::string body) {
    this->_body = body;
}

void        Response::set_file(const std::string file) {
    this->_file = file;
}

void        Response::set_status(std::string code, const std::string msg) {
    this->_status = code;
    this->_status_msg = msg;
}

std::string Response::get_response(void) {
    std::string http_response = this->_header + this->_body;
    return http_response;    
}

/***** member functions *****/
void        Response::respond(int csocket) {
    std::string response = this->_header + this->_body;
    send(csocket, response.c_str(), response.length(), 0);
}

void        Response::printh() {
    std::cout << this->_header;
}