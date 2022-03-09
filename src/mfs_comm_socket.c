
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MPI_PFS.
 *
 *  MPI_PFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MPI_PFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MPI_PFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mfs_comm_socket.h"


//
// Auxiliar
//

#define ONE_MB     (1024 * 1024)
#define MAXPATHLEN (1024)

int mfs_comm_socket_set_default_options ( comm_t *ab )
{
	int    ret, val ;
	struct sockaddr_in client_addr ;

	// set tcp_nodelay
	val = 1 ;
	ret = setsockopt(ab->sd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int)) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        // set sndbuf + rcvbuf
        val = ONE_MB ;
	if (setsockopt(ab->sd, SOL_SOCKET, SO_SNDBUF, (char *) &val, sizeof(int)) == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        val = ONE_MB ;
	if (setsockopt(ab->sd, SOL_SOCKET, SO_RCVBUF, (char *) &val, sizeof(int)) == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}

int mfs_comm_socket_ns_lookup ( char *srv_name, char *port_name )
{
        // Return OK
        return 1 ;
}

int mfs_comm_socket_ns_splithp ( char *port_name, struct hostent **host, int *port )
{
	char *pch ;
	char  srv_host[MAXPATHLEN] ;
	char  srv_port[MAXPATHLEN] ;

	// copy default values...
	strcpy(srv_host, port_name) ;
	strcpy(srv_port, "12345") ;

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
	    mfs_print(DBG_ERROR, "[COMM]: gethostbyname(%s) fails :-(", srv_host) ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}


//
// Init, Finalize
//

int mfs_comm_socket_init ( comm_t *cb, params_t *params )
{
	int val, ret;
	struct sockaddr_in server_addr;

	// new socket
	cb->sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if (cb->sd < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: socket fails :-(") ;
	    perror("socket: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = mfs_comm_socket_set_default_options(cb) ;
	if (ret < 0) {
            return -1 ;
        }

	// sock_reuseaddr
	val = 1;
	ret = setsockopt(cb->sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

	// bind
	bzero((char *)&server_addr, sizeof(server_addr)) ;
	server_addr.sin_family      = AF_INET ;
	server_addr.sin_addr.s_addr = INADDR_ANY ;
	server_addr.sin_port        = htons(12345) ;  // TODO: params->port
	//server_addr.sin_port        = htons(params->port) ;  // TODO: params->port: int

	ret = bind(cb->sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("bind: ") ;
	    return -1 ;
	}

	// listen
	listen(cb->sd, 20);

        // Return OK
        return 1 ;
}

int mfs_comm_socket_finalize ( comm_t *cb )
{
        // Check params...
	if (NULL == cb) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// Close socket
        close(cb->sd) ;
        cb->sd = -1 ;

        // Return OK
        return 1 ;
}


//
// Register, unregister, connect, disconnect
//

int mfs_comm_socket_register ( comm_t *cb )
{
    // TODO

	// save host name and port...
	/* TODO:
	FILE *f;
	char host[255];
	f = fopen(file, "a+");
	if (f != NULL) {
	    gethostname(host, 255);
	    fprintf(f, "%s %s %d\r\n", name, host, port);
	    fclose(f);
	}
	*/

    // Return OK
    return 1 ;
}

int mfs_comm_socket_unregister ( comm_t *cb )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_accept ( comm_t *ab )
{
	int    ret, size, val ;
	struct sockaddr_in client_addr ;

	// accept connection...
	size = sizeof(struct sockaddr_in) ;
	ab->sd = accept(ab->sd, (struct sockaddr *)&client_addr, (socklen_t *)&(size)) ;
	if (ab->sd < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: accept fails :-(") ;
	    perror("accept: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = mfs_comm_socket_set_default_options(ab) ;
	if (ret < 0) {
            return -1 ;
        }

        // Return OK
        return 1 ;
}

int mfs_comm_socket_connect ( comm_t *cb )
{
	int    ret ;
	struct hostent *hp ;
	int    srv_port ;
	struct sockaddr_in server_addr ;

	// translate srv_name -> host + port
	ret = mfs_comm_socket_ns_lookup(cb->srv_name, cb->port_name) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "[COMM]: mfs_comm_socket_lookup fails :-(") ;
            return -1 ;
        }

	ret = mfs_comm_socket_ns_splithp(cb->port_name, &hp, &srv_port) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "[COMM]: mfs_comm_socket_splithp fails :-(") ;
            return -1 ;
        }

	// socket
	cb->sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if (cb->sd < 0) {
	    perror("socket: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = mfs_comm_socket_set_default_options(cb) ;
	if (ret < 0) {
            return -1 ;
        }

	// connect
	bzero((char *)&server_addr, sizeof(server_addr)) ;
	server_addr.sin_family = AF_INET ;
  	server_addr.sin_port   = htons(srv_port) ;
	memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length) ;

	ret = connect(cb->sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) ;
	if (ret == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: connect(%s, %d) fails :-(", srv_host, server_addr.sin_port) ;
	    perror("connect: ") ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}

int mfs_comm_socket_disconnect ( comm_t *cb )
{
    // TODO

    // Return OK
    return 1 ;
}


//
// Send/Receive buffer of data
//

int mfs_comm_socket_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_send_data_to  ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    // TODO

    // Return OK
    return 1 ;
}

