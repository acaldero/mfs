
/*
 *  Copyright 2020-2021 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

    // Read server port...
    ret = mfs_read_server_port(wb->port_name, 0) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: server port name required in the 'mfs_server.port' file.\n", wb->rank) ;
        return -1 ;
    }

    // Connect...
    ret = MPI_Comm_connect(wb->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Comm_connect fails :-(", wb->rank) ;
        return -1 ;
    }

    // recv tag_id
    ret = MPI_Recv(&(wb->tag_id), 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, wb->server, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Recv fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int clientstub_request ( client_stub_t *wb, int req_action, int req_arg1, int req_arg2 )
{
    int ret ;
    int buff[3] ;

    // pack msg fields
    buff[0] = req_action ;
    buff[1] = req_arg1 ;
    buff[2] = req_arg2 ;

    // send msg
    ret = MPI_Send(buff, 3, MPI_INT, 0, wb->tag_id, wb->server) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Send fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int clientstub_finalize ( client_stub_t *wb )
{
    int ret ;

    // Remote disconnect...
    ret = clientstub_request(wb, REQ_ACTION_DISCONNECT, 0, 0) ;
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
    ret = clientstub_request(wb, REQ_ACTION_OPEN, strlen(pathname) + 1, flags) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Send pathname
    ret = MPI_Send(pathname, strlen(pathname) + 1, MPI_CHAR, 0, wb->tag_id, wb->server) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Send fails :-(", wb->rank) ;
        return -1 ;
    }

    // Receive descriptor
    ret = MPI_Recv(&fd, 1, MPI_INT, 0, wb->tag_id, wb->server, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Recv fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return file descriptor
    return fd ;
}

int clientstub_close ( client_stub_t *wb, int fd )
{
    int ret ;

    // Send close msg
    ret = clientstub_request(wb, REQ_ACTION_CLOSE, fd, 0) ;
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
    ret = clientstub_request(wb, REQ_ACTION_READ, fd, count) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Receive data
    ret = MPI_Recv(buff_char, count, MPI_CHAR, 0, wb->tag_id, wb->server, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Recv fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int clientstub_write ( client_stub_t *wb, int fd, void *buff_char, int count )
{
    int ret ;

    // Send write msg
    ret = clientstub_request(wb, REQ_ACTION_WRITE, fd, count) ;
    if (MPI_SUCCESS != ret) {
        return -1 ;
    }

    // Send data
    ret = MPI_Send(buff_char, count, MPI_CHAR, 0, wb->tag_id, wb->server) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Client[%d]: MPI_Send fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

