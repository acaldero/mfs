
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


#include "info_ns.h"


/*
 *  Name server API
 */


// Sockets: get portname + split portname

int info_ns::get_portname ( char *port_name, int sd )
{
	char               my_ip[16] ;
        unsigned int       my_port ;
        struct sockaddr_in my_addr ;
        int                sockfd, ret ;
        socklen_t          len ;

	// initialize variables
        len = sizeof(my_addr) ;
        bzero(&my_addr, len) ;
	my_port = 0 ;

	// get addr+port
        getsockname(sd, (struct sockaddr *)&(my_addr), &(len)) ;
        inet_ntop(AF_INET, &(my_addr.sin_addr), my_ip, sizeof(my_ip)) ;
        my_port = ntohs(my_addr.sin_port) ;

	// if 0.0.0.0 (ANY_ADDRESS) then
        char hostname[1024] ;
        ret = gethostname(hostname, 1024) ;
        if (ret == -1) {
            perror("gethostname: ") ;
	    return -1 ;
        }

	// set portname
	//sprintf(port_name, "%16s:%d", my_ip, my_port) ;
	  sprintf(port_name, "%s:%d",   hostname, my_port) ;

        // Return OK
        return 1 ;
}

int info_ns::split_portname ( char *port_name, struct hostent **host, int *port )
{
	char *pch ;
	char  srv_host[MAXPATHLEN] ;
	char  srv_port[MAXPATHLEN] ;

	// copy default values...
 	 strcpy(srv_host, port_name) ;
	sprintf(srv_port, "%d", NS_DEFAULT_PORT) ;

	// if "host:port" -> host\0port
	pch = strchr(srv_host, ':') ;
	if (NULL != pch) {
	    *pch = '\0' ;
	    pch++ ;
	    strcpy(srv_port, pch) ;
	}

	// translate
	*port = atoi(srv_port) ;
	*host = gethostbyname(srv_host) ;
	if (*host == NULL) {
	    mfs_print(DBG_ERROR, "[COMM]: gethostbyname(%s) fails :-(\n", srv_host) ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}


