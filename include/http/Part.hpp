/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 17:57:21 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/10 08:22:35 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     PART_HPP
# define    PART_HPP

#include <iostream>
#include <map>

class Part
{
private:
    std::string _disposition;
    std::string _type;
    std::string _field;
    std::string _filename;
    std::string _body;
    bool        _file;
public:
    Part();
    ~Part();

    /***** setters *****/
    void        set_field(std::string field);
    void        set_file(std::string file);
    void        set_type(std::string type);
    void        set_dispo(std::string dispo);
    void        set_body(std::string body);
    void        set_isfile();

    /***** getters *****/
    bool        is_file();
};

#endif