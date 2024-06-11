/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:33:48 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/11 16:19:18 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     LOCATION_HPP
# define    LOCATION_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <sstream>

class Location
{
private:
    double                              _max_body_size;
    std::string                         _route;
    std::string                         _root;
    std::vector<std::string>            _index;
    std::vector<std::string>            _methods;
    std::map<std::string, std::string>  _redir;
    bool                                _autoindex;
    std::map<std::string, std::string>  _error_pages;
    std::vector<std::string>            _cgi_ext;
    std::string                         _ret_path;
    std::string                         _cgi_path;

public:
    Location();
    Location(const Location &src);
    ~Location();
    Location& operator=(const Location &rhs);

    /***** getters *****/
    std::string                         getDirective(std::string line);
    std::string                         getRoute(void) const;
    std::string                         getRoot(void) const;
    // std::string getRedir(std::string line);
    std::vector<std::string>            getIndex(void) const;
    std::vector<std::string>            getCgiExt(void) const;
    std::string                         getRetPath(void) const;
    std::string                         getCgiPath(void) const;
    std::map<std::string, std::string>  getErrorPages(void) const;
    std::string                         getMethod(size_t index);
    size_t                              getMethodsSize(void) const;
    double                              getMaxBodySize(void) const;
    std::vector<std::string>            getMethod(void) const;
    void                                setAutoindexToTrue(void);

    /***** member functions *****/    
    void printInfos() const;
    void                                parseRouteLine(std::string line);
    void                                parseRootLine(std::string line);
    void                                parseIndexLine(std::string line);
    void                                parseAllowMethodsLine(std::string line);
    void                                parseClientMaxBodySize(std::string line);
    void                                parseErrorPageLine(std::string line);
    bool                                hasMethods(void) const;
    std::string                         getErrorPage(std::string status, std::string actual);
};

#endif