/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 22:32:10 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 16:19:24 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     REQUEST_HPP
# define    REQUEST_HPP

#include <string>
#include <sstream>
#include <vector>
#include "Part.hpp"

class Request
{
private:
    std::string                         _header;
    std::string                         _body;
    std::string                         _method;
    std::string                         _uri;
    std::string                         _content_type;
    std::string                         _boundary;
    std::string                         _ip_addr;
    int                                 _port;
    size_t                              _content_len;
    bool                                _chunked;
    std::vector<std::string>            _chunks;
    std::vector<Part*>                  _parts;
    std::map<std::string, std::string>  _headerMap;

public:
    Request();
    ~Request();

    /***** getters *****/
    std::string                         get_uri(void) const;
    std::string                         get_bound(void);
    size_t                              get_contLen(void) const;
    int                                 get_port(void) const;
    std::string                         get_ip_addr(void) const;
    std::string                         get_content_length(void) const;
    std::string                         get_content_type(void) const;
    std::string                         get_query_string(void) const;
    std::string                         get_host(void) const;
    std::string                         get_method(void) const;
    std::string                         get_user_agent(void) const;
    std::string                         get_http_protocol(void) const;

    /***** member functions *****/
    void                                body_type();
    void                                set_header(const std::string &head);
    void                                add_chunk(std::string chunk);
    void                                add_part(Part* part);
    void                                parseHeader(); // Parse the header and fill the map
    int                                 has_body() const;
    bool                                chunked() const;
    void                                print_infos();
};

#endif