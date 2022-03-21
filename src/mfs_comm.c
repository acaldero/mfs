
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

#include "mfs_comm.h"


//
// Init, Finalize
//

int mfs_comm_init ( comm_t *cb, conf_part_t *partition, params_t *params )
{
    int ret ;

    // cb->... (stats)
    cb->is_connected = 0 ;
    ret = mfs_comm_stats_reset(cb) ;

    // number of servers
    cb->n_servers = partition->n_nodes ;
    if (cb->n_servers < 0) {
        mfs_print(DBG_ERROR, "[COMM]: set n_servers fails :-(") ;
        return -1 ;
    }

    // Initialization
    cb->comm_protocol = params->comm_backend ;
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
             cb->comm_protocol_name = "SOCKET" ;
	     ret = mfs_comm_socket_init(cb, partition, params->server_port, params->ns_backend) ;
             break ;

        case COMM_USE_MPI:
             cb->comm_protocol_name = "MPI" ;
	     ret = mfs_comm_mpi_init(cb, partition, params->argc, params->argv) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_comm_finalize ( comm_t *cb )
{
    int ret ;

    // Finalize
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_finalize(cb) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_finalize(cb) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // cb->... (stats)
    cb->is_connected = 0 ;

    // Return OK
    return ret ;
}


//
// Register, unregister, connect, disconnect
//

int mfs_comm_register ( comm_t *cb )
{
    int ret ;

    // Open server port...
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_register(cb) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_register(cb) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }
    // Return OK
    return ret ;
}

int mfs_comm_unregister ( comm_t *cb )
{
    int ret ;

    // Unpublish port name
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_unregister(cb) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_unregister(cb) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_comm_accept ( comm_t *ab, comm_t *wb )
{
    int ret ;

    // *ab = *wb ;
    memmove(ab, wb, sizeof(comm_t)) ;

    // Accept
    switch (ab->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_accept(ab) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_accept(ab) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", ab->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // cb->... (stats)
    ab->is_connected = 1 ;

    // Return OK
    return ret ;
}

int mfs_comm_connect ( comm_t *cb, char *srv_uri, int remote_rank )
{
    int ret ;

    // Connect
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_connect(cb, srv_uri, remote_rank) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_connect(cb, srv_uri, remote_rank) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // cb->... (stats)
    cb->is_connected = 1 ;

    // Return OK
    return ret ;
}

int mfs_comm_disconnect ( comm_t *cb, int remote_rank )
{
    int ret ;

    // Disconnect...
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_disconnect(cb, remote_rank) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_disconnect(cb, remote_rank) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // cb->... (stats)
    cb->is_connected = 0 ;

    // Return OK
    return ret ;
}


//
// Stats
//

int mfs_comm_stats_reset ( comm_t *cb )
{
    // cb->... (stats)
    cb->n_send_req = 0 ;
    cb->n_recv_req = 0 ;

    // Return OK
    return 0 ;
}

int mfs_comm_stats_show  ( comm_t *cb, char *prefix )
{
    // Check params...
    if (NULL == cb) {
	return -1 ;
    }

    // Print stats...
    printf("%s: Comm:\n",              prefix) ;
    printf("%s: + # servers=%d\n",     prefix, cb->n_servers) ;
    printf("%s: + # send=%d\n",        prefix, cb->n_send_req) ;
    printf("%s: + # recv=%d\n",        prefix, cb->n_recv_req) ;

    // Return OK
    return 1 ;
}


//
// Send request "header"
//

int mfs_comm_request_send ( comm_t *cb, int rank, long req_action, long req_arg1, long req_arg2 )
{
    int ret ;
    long buff[3] ;

    // pack msg fields
    buff[0] = req_action ;
    buff[1] = req_arg1 ;
    buff[2] = req_arg2 ;

    // send msg
    ret = mfs_comm_send_data_to(cb, rank, buff, 3, MPI_LONG) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Send fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int mfs_comm_request_receive ( comm_t *cb, long *req_action, long *req_arg1, long *req_arg2 )
{
    int ret ;
    long buff[3] ;

    // receive msg
    ret = mfs_comm_recv_data_from(cb, MPI_ANY_SOURCE, buff, 3, MPI_LONG) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Recv fails :-(") ;
        return -1 ;
    }

    // unpack msg fields
    *req_action = buff[0] ;
    *req_arg1   = buff[1] ;
    *req_arg2   = buff[2] ;

    // Return OK/KO
    return ret ;
}


//
// Send/Receive buffer of data
//

int mfs_comm_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;

    // Get CMD message
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_recv_data_from(cb, rank, buff, size, datatype) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_recv_data_from(cb, rank, buff, size, datatype) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // cb->... (stats)
    cb->n_recv_req++ ;

    // Return OK
    return ret ;
}

int mfs_comm_send_data_to  ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;

    // Send answer
    switch (cb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = mfs_comm_socket_send_data_to(cb, rank, buff, size, datatype) ;
             break ;

        case COMM_USE_MPI:
	     ret = mfs_comm_mpi_send_data_to(cb, rank, buff, size, datatype) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on comm_protocol(%d).\n", cb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // cb->... (stats)
    cb->n_send_req++ ;

    // Return OK
    return ret ;
}

