
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MFS.
 *
 *  MFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MFS.  If not, see <http://www.gnu.org/licenses/>.
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

int mfs_comm_socket_serversocket ( int *sd, int port )
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
        ret = mfs_comm_socket_set_default_options((*sd), ONE_MB) ;
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

	ret = bind((*sd), (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: setsockopt fails :-(\n") ;
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

int mfs_comm_socket_init ( comm_t *cb, conf_part_t *partition, int server_port, int ns_backend )
{
	int ret;

        // Check params...
        NULL_PRT_MSG_RET_VAL(cb,        "[COMM]: NULL cb        :-(\n", -1) ;
        NULL_PRT_MSG_RET_VAL(partition, "[COMM]: NULL partition :-(\n", -1) ;

	// Initialize fields
        cb->comm_protocol = COMM_USE_SOCKET ;
        cb->comm_protocol_name = "SOCKET" ;
	cb->rank          = 0 ;
	cb->size          = partition->n_nodes ;
	cb->ns_backend    = ns_backend ;
        cb->is_connected  = 0 ;
        cb->n_send_req    = 0 ;
        cb->n_recv_req    = 0 ;

        // number of servers
        cb->n_servers = partition->n_nodes ;
        if (cb->n_servers < 0) {
            mfs_print(DBG_ERROR, "[COMM]: set n_servers fails :-(\n") ;
            return -1 ;
        }

	// new server socket
        if (server_port != -1)
	{
            ret = mfs_comm_socket_serversocket(&(cb->sd), server_port) ;
	    if (ret < 0) {
	        mfs_print(DBG_ERROR, "[COMM]: socket fails :-(\n") ;
	        perror("socket: ") ;
	        return -1 ;
	    }
	}

	// initialize descriptors for connections
	cb->dd = (int *)malloc((cb->size)*sizeof(int)) ;
	if (NULL == cb->dd) {
	    mfs_print(DBG_ERROR, "[COMM]: malloc fails :-(\n") ;
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

	// Close socket
        ret = mfs_comm_socket_close(&(cb->sd)) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: close socket fails :-(\n") ;
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

int mfs_comm_socket_accept ( comm_t *ab, int remote_rank )
{
	int    ret ;
	int    sd, size, val ;
	struct sockaddr_in client_addr ;

	// accept connection...
	size = sizeof(struct sockaddr_in) ;
	sd = accept(ab->sd, (struct sockaddr *)&(client_addr), (socklen_t *)&(size)) ;
	if (sd < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: accept fails :-(\n") ;
	    perror("accept: ") ;
	    return -1 ;
	}

	// set default options (tcp_nodelay, sndbuff, ...)
        ret = mfs_comm_socket_set_default_options(sd, ONE_MB) ;
	if (ret < 0) {
            return -1 ;
        }

	// set associated socket to rank
	if (remote_rank < 0)
	{
	    for (int i=0; (i<ab->size) && (remote_rank < 0); i++)
	    {
	         if (ab->dd[i] != -1) {
		     remote_rank = i ;
	         }
	    }
	}
	if (remote_rank < 0) {
            return -1 ;
	}

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

	// translate srv_uri -> host + port
	port_name_size = MPI_MAX_PORT_NAME ;
	ret = mfs_ns_lookup(cb, cb->ns_backend, srv_uri, cb->port_name, &port_name_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR,
	              "[COMM]: mfs_comm_socket_lookup fails for '%s' with backend %d :-(\n",
		      srv_uri, cb->ns_backend) ;
            return -1 ;
        }

	ret = mfs_ns_split_portname(cb->port_name, &hp, &srv_port) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "[COMM]: mfs_comm_socket_splithp fails :-(\n") ;
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
	    mfs_print(DBG_ERROR, "[COMM]: connect(%p, %d) fails :-(\n", server_addr.sin_addr, server_addr.sin_port) ;
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

	// If already close then return
	if (-1 == cb->dd[remote_rank]) {
            return 1 ;
	}

	// Close socket
        ret = mfs_comm_socket_close(&(cb->dd[remote_rank])) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: close socket fails :-(\n") ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}


//
// Send/Receive buffer of data
//

int mfs_comm_socket_recv_data_from ( comm_t *cb, int rank, void *buff, int size )
{
        int ret ;

        // Check arguments
	NULL_PRT_MSG_RET_VAL(cb,     "[COMM]: NULL cb     :-(\n", -1) ;
	NULL_PRT_MSG_RET_VAL(cb->dd, "[COMM]: NULL cb->dd :-(\n", -1) ;

        // Try to receive data from...
	ret = -1 ;
	if (MPI_ANY_SOURCE == rank)
	{
	    for (int i=0; i<cb->size; i++)
	    {
	         if (cb->dd[i] != -1) {
	             rank = i ;
		     break ;
	         }
	    }

	    if (rank != -1)
	    {
	        ret = mfs_file_posix_read(cb->dd[rank], buff, size) ;
	        if (ret < 0) {
	    	    mfs_print(DBG_ERROR, "[COMM]: read from socket %d fails :-(\n", cb->dd[rank]) ;
	        }
	    }
	}
	else
	{
	    if (cb->dd[rank] != -1) {
                ret = mfs_file_posix_read(cb->dd[rank], buff, size) ;
	    }
	}

	// Stats
	cb->n_recv_req++ ;

        // Return OK/KO
        return ret ;
}

int mfs_comm_socket_send_data_to  ( comm_t *cb, int rank, void *buff, int size )
{
        int ret ;

        // Check arguments
	NULL_PRT_MSG_RET_VAL(cb,     "[COMM]: NULL cb     :-(\n", -1) ;
	NULL_PRT_MSG_RET_VAL(cb->dd, "[COMM]: NULL cb->dd :-(\n", -1) ;
        if (-1 == cb->dd[rank]) { return -1; }

        // Send data to...
        ret = mfs_file_posix_write(cb->dd[rank], buff, size) ;

        // Stats
        cb->n_send_req++ ;

        // Return OK/KO
        return ret ;
}

//
// Stats
//

int mfs_comm_socket_stats_reset ( comm_t *cb )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(cb, "[COMM]: NULL cb :-(\n", -1) ;

    // cb->... (stats)
    cb->n_send_req = 0 ;
    cb->n_recv_req = 0 ;

    // Return OK
    return 0 ;
}

int mfs_comm_socket_stats_show  ( comm_t *cb, char *prefix )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(cb, "[COMM] NULL cb", -1) ;

    // Print stats...
    printf("%s: Comm:\n",              prefix) ;
    printf("%s: + # servers=%d\n",     prefix, cb->n_servers) ;
    printf("%s: + # send=%d\n",        prefix, cb->n_send_req) ;
    printf("%s: + # recv=%d\n",        prefix, cb->n_recv_req) ;

    // Return OK
    return 1 ;
}

