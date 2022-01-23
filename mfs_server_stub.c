
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


#include "mfs_server_stub.h"


int serverstub_init ( comm_t *wb, int *argc, char ***argv )
{
    int ret ;

    // Initialize
    ret = mfs_comm_init(wb, argc, argv) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails :-(", -1) ;
        return -1 ;
    }

    // Register service
    sprintf(wb->srv_name, "%s.%d", MFS_SERVER_STUB_PNAME, wb->rank) ;

    ret = mfs_comm_register(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: port registration fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_finalize ( comm_t *wb )
{
    int ret ;

    // UnRegister
    ret = mfs_comm_unregister(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: port unregistration fails :-(", wb->rank) ;
        return -1 ;
    }

    // Finalize
    ret = mfs_comm_finalize(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_accept ( comm_t *ab, comm_t *wb )
{
    int ret ;

    // Accept connections...
    ret = mfs_comm_accept(ab, wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: accept connections fails :-(", ab->rank) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_disconnect ( comm_t *ab )
{
    int ret ;

    // Disconnect
    ret = mfs_comm_disconnect(ab) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: disconnect fails :-(", ab->rank) ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File System API
 */

int serverstub_open ( comm_t *ab, int pathname_length, int flags )
{
    int   ret, fd ;
    int   buff_data_len ;
    char *buff_data ;

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
    ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_data + strlen(MFS_DATA_PREFIX), pathname_length, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: file name not received :-(", ab->rank) ;
    }

    // open file
    mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", ab->rank, buff_data) ;
    fd = server_files_open(buff_data, flags) ;

    // send back file descriptor
    ret = mfs_comm_send_data_to(ab, 0, &fd, 1, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: file descriptor cannot be sent :-(", ab->rank) ;
    }

    // free filename buffer
    free(buff_data) ;

    // Return OK/KO
    return fd ;
}

int serverstub_close ( comm_t *ab, int fd )
{
    // close file
    server_files_close(fd) ;

    // Return OK/KO
    return fd ;
}

int serverstub_read ( comm_t *ab, int fd, int count )
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
    ret = mfs_comm_send_data_to(ab, 0, buff_data, count, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(", ab->rank) ;
    }

    // free data buffer
    free(buff_data) ;

    // Return OK/KO
    return ret ;
}

int serverstub_read2 ( comm_t *ab, int fd, int count )
{
    int          ret ;
    char        *buff_data ;
    struct stat  fdstat ;

    // prepare data buffer
    fstat(fd, &fdstat) ;
    buff_data = server_files_mmap(NULL, fdstat.st_size, PROT_READ, MAP_SHARED, fd, 0) ;
    if (NULL == buff_data) {
        mfs_print(DBG_ERROR, "Server[%d]: mmap(%d, ... %d) fails :-(", ab->rank, fd, count) ;
	return -1 ;
    }

    // send data
    ret = mfs_comm_send_data_to(ab, 0, buff_data, count, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(", ab->rank) ;
    }

    // free data buffer
    server_files_munmap(buff_data, fdstat.st_size) ;

    // Return OK/KO
    return ret ;
}

int serverstub_write ( comm_t *ab, int fd, int count )
{
    int   ret ;
    char *buff_data ;

    // prepare data buffer
    buff_data = (char *)malloc(count) ;
    if (NULL == buff_data) {
        mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", ab->rank, count) ;
	return -1 ;
    }

    // receive data
    memset(buff_data, 0, count) ;
    ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_data, count, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", ab->rank) ;
    }

    // write data
    server_files_write(fd, buff_data, count) ;

    // free data buffer
    free(buff_data) ;

    // Return OK/KO
    return ret ;
}

