
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

#include "mfs_comm.h"


//
// Init, Finalize
//

int mfs_comm_init ( comm_t *cb, params_t *params )
{
    int ret, claimed, provided ;

    // cb->... (stats)
    cb->is_connected = 0 ;
    ret = mfs_comm_stats_reset(cb) ;
    ret = mfs_comm_stats_set_nservers(cb, params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[COMM]: set n_servers fails :-(") ;
        return -1 ;
    }

    // MPI_Init
    ret = MPI_Init_thread(params->argc, params->argv, MPI_THREAD_MULTIPLE, &provided) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Init fails :-(") ;
        return -1 ;
    }

    // cb->rank = comm_rank()
    ret = MPI_Comm_rank(MPI_COMM_WORLD, &(cb->rank));
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_rank fails :-(") ;
        return -1 ;
    }

    MPI_Query_thread(&claimed) ;
    if (claimed != provided) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Init_thread with only %s :-/", provided) ;
    }

    // cb->size = comm_size()
    ret = MPI_Comm_size(MPI_COMM_WORLD, &(cb->size));
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_size fails :-(") ;
        return -1 ;
    }

    // cb->status_... 
    cb->status_rank  = -1 ;
    cb->status_tag   = -1 ;
    cb->status_count = -1 ;

    // Return OK
    return 0 ;
}

int mfs_comm_finalize ( comm_t *cb )
{
    int ret ;

    // Finalize
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Finalize fails :-(") ;
        return -1 ;
    }

    // cb->... (stats)
    cb->is_connected = 0 ;

    // Return OK
    return 0 ;
}


//
// Register, unregister, connect, disconnect
//

int mfs_comm_register ( comm_t *cb )
{
    int ret ;

    // Open server port...
    ret = MPI_Open_port(MPI_INFO_NULL, cb->port_name);
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Open_port fails :-(") ;
        return -1 ;
    }

    MPI_Info info ;
    MPI_Info_create(&info) ;
    MPI_Info_set(info, "ompi_global_scope", "true") ;

    ret = MPI_Publish_name(cb->srv_name, info, cb->port_name) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Publish_name fails :-(") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int mfs_comm_unregister ( comm_t *cb )
{
    int ret ;

    // Unpublish port name
    ret = MPI_Unpublish_name(cb->srv_name, MPI_INFO_NULL, cb->port_name) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Unpublish_name fails :-(") ;
        return -1 ;
    }

    // Close port
    MPI_Close_port(cb->port_name) ;

    // Return OK
    return 0 ;
}

int mfs_comm_accept ( comm_t *ab, comm_t *wb )
{
    int ret ;

    // *ab = *wb ;
    memmove(ab, wb, sizeof(comm_t)) ;

    // Accept
    ret = MPI_Comm_accept(ab->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(ab->endpoint)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_accept fails :-(") ;
        return -1 ;
    }

    // cb->... (stats)
    ab->is_connected = 1 ;

    // Return OK
    return 0 ;
}

int mfs_comm_connect ( comm_t *cb )
{
    int ret ;

    ret = MPI_Lookup_name(cb->srv_name, MPI_INFO_NULL, cb->port_name) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Lookup_name fails :-(") ;
        return -1 ;
    }

    // Connect...
    ret = MPI_Comm_connect(cb->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(cb->endpoint)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_connect fails :-(") ;
        return -1 ;
    }

    // cb->... (stats)
    cb->is_connected = 1 ;

    // Return OK
    return 1 ;
}

int mfs_comm_disconnect ( comm_t *cb )
{
    int ret ;

    // Disconnect...
    ret = MPI_Comm_disconnect(&(cb->endpoint)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_disconnect fails :-(") ;
        return -1 ;
    }

    // cb->... (stats)
    cb->is_connected = 0 ;

    // Return OK
    return 1 ;
}


//
// Stats
//

int mfs_comm_stats_set_nservers ( comm_t *cb, params_t *params )
{
    // cb->number_of_process_in_server
    cb->n_servers = params->num_servers ;

    // check negative value...
    if (cb->n_servers < 0) {
        cb->n_servers = 1 ;
	return -1 ;
    }

    // Return OK
    return 0 ;
}

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
    MPI_Status status;

    // Get CMD message
    ret = MPI_Recv(buff, size, datatype, rank, 0, cb->endpoint, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Recv fails :-(") ;
        return -1 ;
    }

    // Copy status
    cb->status_rank = status.MPI_SOURCE ;
    cb->status_tag  = status.MPI_TAG ;
    MPI_Get_count(&status, datatype, &(cb->status_count));

    // cb->... (stats)
    cb->n_recv_req++ ;

    // Return OK
    return 1 ;
}

int mfs_comm_send_data_to  ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;

    // Send answer
    ret = MPI_Send(buff, size, datatype, rank, 0, cb->endpoint) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Send fails :-(") ;
        return -1 ;
    }

    // cb->... (stats)
    cb->n_send_req++ ;

    // Return OK
    return 1 ;
}

