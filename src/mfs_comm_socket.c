
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
// Auxiliar: sockets
//

int mfs_comm_socket_set_default_options ( int ab, int buf_size )
{
	int    ret, val ;
	struct sockaddr_in client_addr ;

	// set tcp_nodelay
	val = 1 ;
	ret = setsockopt(ab, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int)) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        // set sndbuf + rcvbuf
        val = buf_size ;
	if (setsockopt(ab, SOL_SOCKET, SO_SNDBUF, (char *) &val, sizeof(int)) == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        val = buf_size ;
	if (setsockopt(ab, SOL_SOCKET, SO_RCVBUF, (char *) &val, sizeof(int)) == -1) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}

int mfs_comm_socket_serversocket ( int *sd, int port )
{
	int val, ret;
	struct sockaddr_in server_addr;

	// new socket
	(*sd) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if ((*sd) < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: socket fails :-(") ;
	    perror("socket: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = mfs_comm_socket_set_default_options((*sd), ONE_MB) ;
	if (ret < 0) {
            return -1 ;
        }

	// sock_reuseaddr
	val = 1;
	ret = setsockopt((*sd), SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("setsockopt: ") ;
	    return -1 ;
	}

	// bind
	bzero((char *)&server_addr, sizeof(server_addr)) ;
	server_addr.sin_family      = AF_INET ;
	server_addr.sin_addr.s_addr = INADDR_ANY ;
	server_addr.sin_port        = htons(port) ;

	ret = bind((*sd), (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(") ;
	    perror("bind: ") ;
	    return -1 ;
	}

	// listen
	listen((*sd), 20);

        // Return OK
        return 1 ;
}

int mfs_comm_socket_close ( int *sd )
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


//
// Init, Finalize
//

int mfs_comm_socket_init ( comm_t *cb, params_t *params, conf_part_t *partition )
{
	int ret;

        // Check params...
	if (NULL == cb) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// new socket
        ret = mfs_comm_socket_serversocket(&(cb->sd), 12345) ; // TODO: params->port ?
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: socket fails :-(") ;
	    perror("socket: ") ;
	    return -1 ;
	}

	// initialize fields
	cb->rank = 0 ;
	cb->size = partition->n_nodes ;

	cb->dd = (int *)malloc((cb->size)*sizeof(int)) ;
	if (NULL == cb->dd) {
	    mfs_print(DBG_ERROR, "[COMM]: malloc fails :-(") ;
	    perror("malloc: ") ;
	    return -1 ;
	}

	for (int i=0; i<cb->size; i++) {
	     cb->dd[i] = -1 ;
	}

        // Return OK
        return 1 ;
}

int mfs_comm_socket_finalize ( comm_t *cb )
{
	int ret ;

        // Check params...
	if (NULL == cb) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// Close socket
        ret = mfs_comm_socket_close(&(cb->sd)) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: close socket fails :-(") ;
	    return -1 ;
	}

	// finalize fields
	free(cb->dd) ;
	cb->dd = NULL ;

        // Return OK
        return 1 ;
}


//
// Register, unregister, connect, disconnect
//

int mfs_comm_socket_register ( comm_t *cb )
{
	int ret ;

        // Check params...
	if (NULL == cb) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// get port_name
	ret = mfs_ns_get_portname(cb->port_name, cb->sd) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "[COMM]: mfs_ns_get_portname fails :-(") ;
            return -1 ;
        }

	// register service into ns
	ret = mfs_ns_insert(cb, NS_USE_DBM, cb->srv_name, cb->port_name) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: registration fails :-(") ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}

int mfs_comm_socket_unregister ( comm_t *cb )
{
	int ret ;

        // Check params...
	if (NULL == cb) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// unregister service from ns
	ret = mfs_ns_remove(cb, NS_USE_DBM, cb->srv_name) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: unregistration fails :-(") ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}

int mfs_comm_socket_accept ( comm_t *ab )
{
	int    ret ;
	int    sd, size, val ;
	struct sockaddr_in client_addr ;

        // Check params...
	if (NULL == ab) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// accept connection...
	size = sizeof(struct sockaddr_in) ;
	sd = accept(ab->sd, (struct sockaddr *)&(client_addr), (socklen_t *)&(size)) ;
	if (sd < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: accept fails :-(") ;
	    perror("accept: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = mfs_comm_socket_set_default_options(sd, ONE_MB) ;
	if (ret < 0) {
            return -1 ;
        }

	// set associated socket to rank
	int remote_rank = 0 ; // TODO: get remote rank
	ab->dd[remote_rank] = sd ;

        // Return OK
        return 1 ;
}

int mfs_comm_socket_connect ( comm_t *cb, char *srv_uri, int remote_rank )
{
	int    ret, sd ;
	struct hostent *hp ;
	int    srv_port, port_name_size ;
	struct sockaddr_in server_addr ;

        // Check params...
	if (NULL == cb) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// translate srv_uri -> host + port
	port_name_size = MPI_MAX_PORT_NAME ;
	ret = mfs_ns_lookup(cb, NS_USE_DBM, srv_uri, cb->port_name, &port_name_size) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "[COMM]: mfs_comm_socket_lookup fails :-(") ;
            return -1 ;
        }

	ret = mfs_ns_split_portname(cb->port_name, &hp, &srv_port) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "[COMM]: mfs_comm_socket_splithp fails :-(") ;
            return -1 ;
        }

	// socket
	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if (sd < 0) {
	    perror("socket: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = mfs_comm_socket_set_default_options(sd, ONE_MB) ;
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
	    mfs_print(DBG_ERROR, "[COMM]: connect(%p, %d) fails :-(", server_addr.sin_addr, server_addr.sin_port) ;
	    perror("connect: ") ;
	    return -1 ;
	}

	// set associated socket to rank
	cb->dd[remote_rank] = sd ;

        // Return OK
        return 1 ;
}

int mfs_comm_socket_disconnect ( comm_t *cb, int remote_rank )
{
	int ret ;

        // Check params...
	if (NULL == cb) {
	    mfs_print(DBG_ERROR, "[COMM]: NULL argument :-(") ;
	    return -1 ;
	}

	// If already close then return
	if (-1 == cb->dd[remote_rank]) {
            return 1 ;
	}

	// Close socket
        ret = mfs_comm_socket_close(&(cb->dd[remote_rank])) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: close socket fails :-(") ;
	    return -1 ;
	}

	cb->dd[remote_rank] = -1 ;

        // Return OK
        return 1 ;
}


//
// Send/Receive buffer of data
//

int mfs_comm_socket_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;
    int buff_size ;

    // Check arguments
    if (NULL == cb)         { return -1; }
    if (NULL == cb->dd)     { return -1; }
    if (-1 == cb->dd[rank]) { return -1; }

    // Send data to...
    MPI_Type_size(datatype, &buff_size) ;
    ret = mfs_file_read(cb->dd[rank], buff, buff_size) ;

    // Return OK/KO
    return ret ;
}

int mfs_comm_socket_send_data_to  ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;
    int buff_size ;

    // Check arguments
    if (NULL == cb)         { return -1; }
    if (NULL == cb->dd)     { return -1; }
    if (-1 == cb->dd[rank]) { return -1; }

    // Send data to...
    MPI_Type_size(datatype, &buff_size) ;
    ret = mfs_file_write(cb->dd[rank], buff, buff_size) ;

    // Return OK/KO
    return ret ;
}

