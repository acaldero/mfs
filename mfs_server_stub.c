
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


#include "mfs_server_stub.h"


int serverstub_init ( server_stub_t *wb, int *argc, char ***argv )
{
    int ret, claimed, provided ;

    // MPI_Init
    ret = MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Init fails :-(", -1) ;
        return -1 ;
    }

    // wb->rank = comm_rank()
    ret = MPI_Comm_rank(MPI_COMM_WORLD, &(wb->rank));
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Comm_rank fails :-(", wb->rank) ;
        return -1 ;
    }

    MPI_Query_thread(&claimed) ;
    if (claimed != provided) {
        mfs_print(DBG_WARNING, "Server[%d]: MPI_Init_thread with only %s :-/", wb->rank, provided) ;
    }

    // wb->size = comm_size()
    ret = MPI_Comm_size(MPI_COMM_WORLD, &(wb->size));
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Comm_size fails :-(", wb->rank) ;
        return -1 ;
    }

    // Open server port...
    ret = MPI_Open_port(MPI_INFO_NULL, wb->port_name);
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Open_port fails :-(", wb->rank) ;
        return -1 ;
    }


    // Publish port name
    sprintf(wb->srv_name, "%s.%d", MFS_SERVER_STUB_PNAME, wb->rank) ;

    MPI_Info info ;
    MPI_Info_create(&info) ;
    MPI_Info_set(info, "ompi_global_scope", "true") ;

    ret = MPI_Publish_name(wb->srv_name, info, wb->port_name) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Publish_name fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_finalize ( server_stub_t *wb )
{
    int ret ;

    // Close port
    MPI_Close_port(wb->port_name) ;

    // Unpublish port name
    ret = MPI_Unpublish_name(wb->srv_name, MPI_INFO_NULL, wb->port_name) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Unpublish_name fails :-(", wb->rank) ;
        return -1 ;
    }

    // Finalize
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Finalize fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_accept ( server_stub_t *ab, server_stub_t *wb )
{
    int ret ;

    // *ab = *wb ;
    memmove(ab, wb, sizeof(server_stub_t)) ;

    // Accept
    ret = MPI_Comm_accept(ab->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(ab->client)) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "Server[%d]: MPI_Comm_accept fails :-(", ab->rank) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_disconnect ( server_stub_t *ab )
{
    int ret ;

    // Disconnect
    ret = MPI_Comm_disconnect(&(ab->client)) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int serverstub_request_recv ( server_stub_t *ab, void *buff, int size, int datatype )
{
    int ret ;
    MPI_Status status;

    // Get CMD message
    ret = MPI_Recv(buff, size, datatype, MPI_ANY_SOURCE, 0, ab->client, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "Server[%d]: MPI_Recv fails :-(", ab->rank) ;
    }

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int serverstub_request_send ( server_stub_t *ab, void *buff, int size, int datatype )
{
    int ret ;

    // Send answer
    ret = MPI_Send(buff, size, datatype, 0, 0, ab->client) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "Server[%d]: MPI_Send fails :-(", ab->rank) ;
    }

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}


/*
 *  File System API
 */

int serverstub_open ( server_stub_t *ab, int pathname_length, int flags )
{
    int   ret, fd ;
    int   buff_data_len ;
    char *buff_data ;
    MPI_Status status;

    // prepare filename buffer
    buff_data_len = strlen(MFS_DATA_PREFIX) + pathname_length + 1 ;
    buff_data     = (char *)malloc(buff_data_len) ;
    if (NULL == buff_data) {
        mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", ab->rank, buff_data_len) ;
	return -1 ;
    }

    // read filename
    memset(buff_data, 0, buff_data_len) ;
    strcpy(buff_data, MFS_DATA_PREFIX) ;
    ret = MPI_Recv(buff_data + strlen(MFS_DATA_PREFIX), pathname_length, MPI_CHAR, MPI_ANY_SOURCE, 0, ab->client, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "Server[%d]: MPI_Recv fails :-(", ab->rank) ;
    }

    // open file
    mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", ab->rank, buff_data) ;
    fd = server_files_open(buff_data, flags) ;

    // send back file descriptor
    ret = MPI_Send(&fd, 1, MPI_INT, 0, 0, ab->client) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "Server[%d]: MPI_Send fails :-(", ab->rank) ;
    }

    // free filename buffer
    free(buff_data) ;

    // Return OK/KO
    return fd ;
}

int serverstub_close ( server_stub_t *ab, int fd )
{
    // close file
    server_files_close(fd) ;

    // Return OK/KO
    return fd ;
}

int serverstub_read ( server_stub_t *ab, int fd, int count )
{
    int   ret ;
    char *buff_data ;

    // prepare data buffer
    buff_data = (char *)malloc(count) ;
    if (NULL == buff_data) {
        mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", ab->rank, count) ;
	return -1 ;
    }

    // read data
    memset(buff_data, 0, count) ;
    server_files_read(fd, buff_data, count) ;

    // send data
    ret = MPI_Send(buff_data, count, MPI_CHAR, 0, 0, ab->client) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "Server[%d]: MPI_Send fails :-(", ab->rank) ;
    }

    // free data buffer
    free(buff_data) ;

    // Return OK/KO
    return ret ;
}

int serverstub_write ( server_stub_t *ab, int fd, int count )
{
    int   ret ;
    char *buff_data ;
    MPI_Status status;

    // prepare data buffer
    buff_data = (char *)malloc(count) ;
    if (NULL == buff_data) {
        mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", ab->rank, count) ;
	return -1 ;
    }

    // receive data
    memset(buff_data, 0, count) ;
    ret = MPI_Recv(buff_data, count, MPI_CHAR, MPI_ANY_SOURCE, 0, ab->client, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "Server[%d]: MPI_Recv fails :-(", ab->rank) ;
    }

    // write data
    server_files_write(fd, buff_data, count) ;

    // free data buffer
    free(buff_data) ;

    // Return OK/KO
    return ret ;
}

