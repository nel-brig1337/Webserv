/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:33:49 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/14 15:50:18 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "WSNetworking.hpp"

class Cgi
{
    private:
        MainClient *main_client;
    public:
        Cgi(MainClient *main_client);
        ~Cgi();
        void just_print();
    // class CgiException : public std::exception
    // {
    //     private:
    //         std::string _msg;
    //     public:
    //         CgiException(std::string msg) : _msg(msg) {}
    //         virtual ~CgiException() throw() {}
    //         virtual const char *what() const throw() { return _msg.c_str(); }
    // };
}; 

#endif // CGI_HPP