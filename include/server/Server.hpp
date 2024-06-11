/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:33:57 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/10 12:26:38 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     SERVER_HPP
# define    SERVER_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <fstream>
# include "Location.hpp"

class Server
{
private:
    std::vector<std::string>                            _methods;

    std::map<std::string, std::vector<std::string>>     _sockets;
    std::vector<std::string>                            _server_name;
    
    double                                              _max_body_size;
    std::vector<std::string>                            _index;
    std::string                                         _root;
    std::map<std::string, std::string>                  _error_pages;
    std::vector<Location*>                              _locations;
    bool                                                _autoindex;
    std::vector<std::string>                            _cgi_ext;
    std::string                                         _ret_path;
    std::string                                         _cgi_path;
public:
    Server();
    Server(const Server &src);
    ~Server();
    Server& operator=(const Server &rhs);

    /***** testing *****/
    void printInfos(void) const;

    /***** getters *****/
    Location*                                           getLocation(size_t index) const;
    size_t                                              getLocationsSize() const;
    std::string                                         getMethod(size_t index) const;
    size_t                                              getMethodsSize(void) const;
    size_t                                              getSocketsSize(void) const;
    double                                              getMaxBodySize(void) const;
    const std::map<std::string, std::vector<std::string>>&     getSockets(void) const;
    const std::vector<std::string>&                            getServerName(void) const;

    /***** setters *****/
    void                                                setAutoindexToTrue(void);

    /***** parsing *****/
    void                                                parser(std::ifstream& file);
    std::vector<int>                                    getPorts(void);

    int                                                 parseListenLine(std::string line);
    std::string                                         parseRootLine(std::string line);
    void                                                parseServerNameLine(std::string line);
    void                                                parseLocationLine(std::ifstream& file, std::string line);
    void                                                parseIndexLine(std::string line);
    void                                                parseErrorPageLine(std::string line);
    void                                                parseClientMaxBodySizeLine(std::string line);
    void                                                parseAllowMethodsLine(std::string line);

    /***** utils *****/
    bool                                                hasMethods(void) const;
    std::string                                         getErrorPage(std::string status, std::string actual);
};

#endif