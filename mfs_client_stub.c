
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


#include "mfs_client_stub.h"


int clientstub_init ( client_stub_t *wb, int *argc, char ***argv )
{
    int ret, claimed, provided ;
    MPI_Status status ;

    // MPI_Init
    ret = MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Init fails :-(", wb->rank) ;
        return -1 ;
    }

    MPI_Query_thread(&claimed) ;
    if (claimed != provided) {
        mfs_print(DBG_WARNING, "Client[%d]: MPI_Init_thread with only %s :-(", wb->rank, provided) ;
    }

    // wb->size = comm_size()
    ret = MPI_Comm_size(MPI_COMM_WORLD, &(wb->size)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Comm_size fails :-(", wb->rank) ;
        return -1 ;
    }

    // wb->rank = comm_rank()
    ret = MPI_Comm_rank(MPI_COMM_WORLD, &(wb->rank)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Comm_rank fails :-(", wb->rank) ;
        return -1 ;
    }

    // Lookup port name
    sprintf(wb->srv_name, "%s.%d", MFS_SERVER_STUB_PNAME, wb->rank) ;

    ret = MPI_Lookup_name(wb->srv_name, MPI_INFO_NULL, wb->port_name) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Lookup_name fails :-(", wb->rank) ;
        return -1 ;
    }

    // Connect...
    ret = MPI_Comm_connect(wb->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Comm_connect fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int clientstub_finalize ( client_stub_t *wb )
{
    int ret ;

    // Remote disconnect...
    ret = mfs_comm_request_send(wb->server, 0, REQ_ACTION_DISCONNECT, 0, 0) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Disconnect...
    ret = MPI_Comm_disconnect(&(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Comm_disconnect fails :-(", wb->rank) ;
        return -1 ;
    }

    // Finalize...
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Finalize fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int clientstub_open ( client_stub_t *wb, const char *pathname, int flags )
{
    int ret, fd ;
    MPI_Status status ;

    // Send open msg
    ret = mfs_comm_request_send(wb->server, 0, REQ_ACTION_OPEN, strlen(pathname) + 1, flags) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Send pathname
    ret = mfs_comm_send_data_to(wb->server, 0, (void *)pathname, strlen(pathname) + 1, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(", wb->rank) ;
        return -1 ;
    }

    // Receive descriptor
    ret = mfs_comm_recv_data_from(wb->server, 0, &fd, 1, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: file descriptor not received :-(", wb->rank) ;
        return -1 ;
    }

    // Return file descriptor
    return fd ;
}

int clientstub_close ( client_stub_t *wb, int fd )
{
    int ret ;

    // Send close msg
    ret = mfs_comm_request_send(wb->server, 0, REQ_ACTION_CLOSE, fd, 0) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int clientstub_read ( client_stub_t *wb, int fd, void *buff_char, int count )
{
    int ret ;
    MPI_Status status ;

    // Send read msg
    ret = mfs_comm_request_send(wb->server, 0, REQ_ACTION_READ, fd, count) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Receive data
    ret = mfs_comm_recv_data_from(wb->server, 0, buff_char, count, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: data not received :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int clientstub_write ( client_stub_t *wb, int fd, void *buff_char, int count )
{
    int ret ;

    // Send write msg
    ret = mfs_comm_request_send(wb->server, 0, REQ_ACTION_WRITE, fd, count) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Send data
    ret = mfs_comm_send_data_to(wb->server, 0, buff_char, count, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: data cannot be sent :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

