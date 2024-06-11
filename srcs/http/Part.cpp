/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 15:38:46 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/10 08:22:40 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http/Part.hpp"

Part::Part() : _file(false) {}

Part::~Part() {}

/***** setters *****/
void    Part::set_field(std::string field) {this->_field = field;}

void    Part::set_file(std::string file) {this->_filename = file;}

void    Part::set_type(std::string type) {this->_type = type;}

void    Part::set_dispo(std::string dispo) {this->_disposition = dispo;}

void    Part::set_body(std::string body) {this->_body = body;}

void    Part::set_isfile() {this->_file = true;}

/***** setters *****/
bool    Part::is_file() {return this->_file;}