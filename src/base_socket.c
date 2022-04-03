
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#include "base_socket.h"


//
// API
//

int base_socket_set_default_options ( int ab, int buf_size )
{
	int    ret, val ;
	struct sockaddr_in client_addr ;

	// set tcp_nodelay
	val = 1 ;
	ret = setsockopt(ab, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int)) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(\n") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        // set sndbuf + rcvbuf
        val = buf_size ;
	if (setsockopt(ab, SOL_SOCKET, SO_SNDBUF, (char *) &val, sizeof(int)) == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(\n") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        val = buf_size ;
	if (setsockopt(ab, SOL_SOCKET, SO_RCVBUF, (char *) &val, sizeof(int)) == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(\n") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}

int base_socket_serversocket ( int *sd, int port )
{
	int val, ret;
	struct sockaddr_in server_addr;

	// new socket
	(*sd) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if ((*sd) < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: socket fails :-(\n") ;
	    perror("socket: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = base_socket_set_default_options((*sd), ONE_MB) ;
	if (ret < 0) {
            return -1 ;
        }

	// sock_reuseaddr
	val = 1;
	ret = setsockopt((*sd), SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(\n") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

	// bind
	bzero((char *)&server_addr, sizeof(server_addr)) ;
	server_addr.sin_family      = AF_INET ;
	server_addr.sin_addr.s_addr = INADDR_ANY ;
	server_addr.sin_port        = htons(port) ;

	mfs_print(DBG_INFO, "[COMM]: bind(AF_INET, INADDR_ANY, %d)\n", htons(port)) ;
	ret = bind((*sd), (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: bind fails :-(\n") ;
	    perror("bind: ") ;
	    return -1 ;
	}

	// listen
	listen((*sd), 20);

        // Return OK
        return 1 ;
}

int base_socket_close ( int *sd )
{
	// Try to close (if it is not already closed)
	if ((*sd) != -1)
	{
            close((*sd)) ;
            (*sd) = -1 ;
	}

        // Return OK
        return 1 ;
}

int base_socket_accept ( int sd )
{
	int    ret ;
	int    ad, size, val ;
	struct sockaddr_in client_addr ;

	// accept connection...
	size = sizeof(struct sockaddr_in) ;
	ad = accept(sd, (struct sockaddr *)&(client_addr), (socklen_t *)&(size)) ;
	if (ad < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: accept fails :-(\n") ;
	    perror("accept: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = base_socket_set_default_options(ad, ONE_MB) ;
	if (ret < 0) {
            return -1 ;
        }

        // Return accepted socket
        return ad ;
}

int base_socket_connect ( struct hostent *hp, int srv_port )
{
	int    ret, sd ;
	struct sockaddr_in server_addr ;

	// socket
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if (sd < 0) {
	    perror("socket: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = base_socket_set_default_options(sd, ONE_MB) ;
	if (ret < 0) {
            return -1 ;
        }

	// connect
	bzero((char *)&server_addr, sizeof(server_addr)) ;
	server_addr.sin_family = AF_INET ;
  	server_addr.sin_port   = htons(srv_port) ;
	memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length) ;

	ret = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) ;
	if (ret == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: connect(%p, %d) fails :-(\n", server_addr.sin_addr, server_addr.sin_port) ;
	    perror("connect: ") ;
	    return -1 ;
	}

        // Return connected socket
        return sd ;
}

