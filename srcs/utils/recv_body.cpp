/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recv_body.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:00:48 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/07 11:11:41 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Core.hpp"

// voir pour stocker part body en chunk pour secu si file
void        recv_multipart(int socket, Request *req, Response *res, size_t mbs) {
    std::string         del = "--" + req->get_bound();
    std::string         end_del = del + "--";
    std::string         line;
    size_t              total_recv = 0;
    
    while ((line = read_line(socket, res)) != end_del) {
        if (line == del) {
            Part        *part = new Part;
            std::string part_data;
            
            part_headers(socket, part, res);
            while ((line = read_line(socket, res)) != del && line != end_del) {
                if (!part->is_file())
                    line += "\r\n";
                part_data += line;
            }
            part->set_body(part_data);
            total_recv += part_data.size();
            req->add_part(part);

            if (mbs > 0 && total_recv > mbs) {
                res->set_status("413", " Payload too large");
                throw Core::NotMatchingConf();
            }
            if (line == end_del) break;
        }
    }
}

void        recv_chunked(int socket, Request *req, Response *res, size_t mbs) {
    size_t  total_recv = 0;

    while (true) {
        std::string chunk_head = read_line(socket, res);
        size_t      chunk_size = std::stoi(chunk_head, nullptr, 16);
        char        buffer[CHUNK_SIZE];

        if (chunk_size == 0)
            break;
        
        while (chunk_size > 0) {
            size_t  to_read = std::min(chunk_size, (size_t)CHUNK_SIZE);
            ssize_t received = recv(socket, buffer, to_read, 0);
            
            if (received <= 0) {
                if (received == 0)
                    res->set_status("400", " Bad Request");
                else
                    res->set_status("500", " Internal Server Error");
                throw Core::BodyRecvError();
            }
            req->add_chunk(std::string(buffer, received));
            total_recv += received;
            chunk_size -= received;

            if (mbs > 0 && total_recv > mbs) {
                res->set_status("413", " Payload too large");
                throw Core::NotMatchingConf();
            }
        }
        read_line(socket, res);
    }
}

void        recv_contLen(int socket, Request *req, Response *res, size_t mbs) {
    size_t  ctl = req->get_contLen();
    size_t  total_recv = 0;
    char    buffer[CHUNK_SIZE];

    if (ctl > mbs) {
        res->set_status("413", " Payload Too Large");
        throw Core::NotMatchingConf();
    }

    while (total_recv < ctl) {
        size_t to_read = std::min(ctl - total_recv, (size_t)CHUNK_SIZE);
        ssize_t received = recv(socket, buffer, to_read, 0);
        
        if (received <= 0) {
            if (received == 0)
                res->set_status("400", " Bad Request");
            else
                res->set_status("500", " Internal Server Error");
            throw Core::BodyRecvError();
        }
        req->add_chunk(std::string(buffer, received));
        total_recv += received;

        if (mbs > 0 && total_recv > mbs) {
            res->set_status("413", " Payload Too Large");
            throw Core::NotMatchingConf();
        }
    }
}

void        part_headers(int client_socket, Part *part, Response *res) {
    std::string line;
    
    while (!(line = read_line(client_socket, res)).empty()) {
        if (line.find("Content-Disposition:")) {
            size_t      end_pos = line.find(";");
            std::string new_line = line.substr(end_pos, line.length());

            part->set_dispo(line.substr(21, end_pos));
            end_pos = new_line.find(";");
            if (end_pos != std::string::npos) {
                part->set_field(new_line.substr(1, end_pos));
                new_line = new_line.substr(end_pos, line.length());
                part->set_file(new_line.substr(1, new_line.length()));
                part->set_isfile();
            } else
                part->set_field(new_line.substr(1, new_line.length()));
        } else if (line.find("Content-Type:")) {
            part->set_type(line.substr(14, line.length()));
        }
    }
}