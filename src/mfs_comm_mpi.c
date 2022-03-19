
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

#include "mfs_comm_mpi.h"


//
// Init, Finalize
//

int mfs_comm_mpi_init ( comm_t *cb, params_t *params, conf_part_t *partition )
{
    int ret, claimed, provided ;

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
    return 1 ;
}

int mfs_comm_mpi_finalize ( comm_t *cb )
{
    int ret ;

    // Finalize
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Finalize fails :-(") ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}


//
// Register, unregister, connect, disconnect
//

int mfs_comm_mpi_register ( comm_t *cb )
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
    return 1 ;
}

int mfs_comm_mpi_unregister ( comm_t *cb )
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
    return 1 ;
}

int mfs_comm_mpi_accept ( comm_t *ab )
{
    int ret ;

    // Accept
    ret = MPI_Comm_accept(ab->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(ab->endpoint)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_accept fails :-(") ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int mfs_comm_mpi_connect ( comm_t *cb, char *srv_uri, int remote_rank )
{
    int ret ;

    ret = MPI_Lookup_name(srv_uri, MPI_INFO_NULL, cb->port_name) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Lookup_name fails :-(") ;
        return -1 ;
    }

    // Connect...
    ret = MPI_Comm_connect(cb->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(cb->endpoint)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_connect(%d) fails :-(", remote_rank) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int mfs_comm_mpi_disconnect ( comm_t *cb, int remote_rank )
{
    int ret ;

    // Disconnect...
    ret = MPI_Comm_disconnect(&(cb->endpoint)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Comm_disconnect(%d) fails :-(", remote_rank) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}


//
// Send/Receive buffer of data
//

int mfs_comm_mpi_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;
    MPI_Status status;
    int msg_size ;

    // Check if there is a CMD message
    ret = MPI_Probe(rank, 0, cb->endpoint, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Probe fails :-(") ;
        return -1 ;
    }

    MPI_Get_count(&status, datatype, &msg_size) ;
    if (msg_size != size) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Probe detects an unexpected message of %d (but not %d):-(", msg_size, size) ;
        return -1 ;
    }

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

    // Return OK
    return 1 ;
}

int mfs_comm_mpi_send_data_to  ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;

    // Send answer
    ret = MPI_Send(buff, size, datatype, rank, 0, cb->endpoint) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Send fails :-(") ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

