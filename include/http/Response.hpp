/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 17:34:44 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 09:26:03 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     RESPONSE_HPP
# define    RESPONSE_HPP

#include <string>

class Response
{
private:
    std::string _file;
    std::string _header;
    std::string _body;
    std::string _status_msg;
    std::string _content_type;
    std::string _status;
    int         _content_len;
    
public:
    Response();
    ~Response();

    /***** getters *****/
    std::string get_status();
    std::string get_file();

    /***** setters *****/
    void        set_head();
    void        set_body(const std::string body);
    void        set_file(const std::string file);
    void        set_status(std::string code, const std::string msg);
    std::string get_response(void);

    /***** process *****/
    void        respond(int csocket);

    /***** utils *****/
    void        printh();
};

std::string     content_type(const std::string& filename);

#endif