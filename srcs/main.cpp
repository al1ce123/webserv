/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nlence-l <nlence-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 16:37:10 by jlecorne          #+#    #+#             */
/*   Updated: 2024/06/10 13:15:56 by nlence-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server/Core.hpp"

int main(int ac, char **av) {
    (void)av;
    if (ac == 2)
    {
        try {
            Core *core = new Core;
            std::string file_name(av[1]);

            core->core_parser(file_name);
            core->webserv();

        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
    else {
        std::cerr << GREY << "webserv: " << RED << "ERROR" << GREY << ": Bad argument" << std::endl;
        std::cerr << GREY << "usage: ./webserv [configuration file]" << std::endl;
        return 1;
    }
    return 0;
} /* main */
