
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of XPNlite.
 *
 *  XPNlite is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPNlite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with XPNlite.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __BASE_SOCKET_H__
#define __BASE_SOCKET_H__

    // Includes
    #include "base_lib.h"

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <netdb.h>


    // Consts
    #define ONE_MB     (1024 * 1024)


    // API
    int base_socket_set_default_options ( int  ab, int buf_size ) ;

    int base_socket_serversocket        ( int *sd, int port ) ;
    int base_socket_close               ( int *sd ) ;

    int base_socket_accept              ( int  sd ) ;
    int base_socket_connect             ( struct hostent *hp, int srv_port ) ;

#endif

