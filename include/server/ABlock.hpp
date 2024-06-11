// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   ABlock.hpp                                         :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: jlecorne <jlecorne@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/04/16 18:07:19 by jlecorne          #+#    #+#             */
// /*   Updated: 2024/06/06 15:22:23 by jlecorne         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #ifndef ABLOCK_HPP
// # define ABLOCK_HPP

// #include <vector>
// #include <map>
// #include <string>

// class ABlock
// {
// protected:
//     // Server and Location hasn't
//     std::vector<std::string>            _cgi_ext;
//     std::string                         _ret_path;
//     std::string                         _cgi_path;
//     bool                                _autoindex; //-> location has

//     //server has
//     std::vector<std::string>            _methods;
//     std::string                         _root;
//     std::string                         _index;
//     size_t                              _max_bsize;
//     std::map<std::string, std::string>  _errPath;
//     bool                                _autoindex; //-> location has
// public:
//     ABlock();
//     ABlock(const ABlock *src);
//     virtual ~ABlock() = 0;
//     ABlock &operator=(const ABlock *rhs);

//     /***** getters *****/
//     std::string                         get_errPath(std::string status, std::string actual);
//     std::string                         get_method(size_t i);
//     size_t                              get_mSize();
//     size_t                              get_mbs();

//     /***** checks *****/
//     bool                                has_methods() const;
// };

// #endif