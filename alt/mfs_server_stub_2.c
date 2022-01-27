
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


/*
 *  Server stub API
 */

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
    int   ret ;
    int   fd ;
    int   buff_data_len ;
    char *buff_data ;

    ret           = 0 ;
    buff_data     = NULL ;
    buff_data_len = strlen(MFS_DATA_PREFIX) + pathname_length + 1 ;

    // prepare filename buffer
    // if (ret >= 0)
    {
        ret = mfs_malloc(&buff_data, buff_data_len) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", ab->rank, buff_data_len) ;
        }
    }

    // read filename
    if (ret >= 0)
    {
        strcpy(buff_data, MFS_DATA_PREFIX) ;
        ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE,
                                      buff_data + strlen(MFS_DATA_PREFIX), pathname_length, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file name not received :-(", ab->rank) ;
        }
    }

    // open file
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", ab->rank, buff_data) ;
        ret = fd = server_files_open(buff_data, flags) ;
        if (fd < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file not opened :-(", ab->rank) ;
        }
    }

    // send back file descriptor
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, &fd, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file descriptor cannot be sent :-(", ab->rank) ;
        }
    }

    // free filename buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", ab->rank) ;
        }
    }

    // Return OK/KO
    return fd ;
}

int serverstub_close ( comm_t *ab, int fd )
{
    int ret ;

    ret = 0 ;

    // close file
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'close' for file %d\n", ab->rank, fd) ;
        ret = server_files_close(fd) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: close(%d) fails :-(", ab->rank, fd) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_read ( comm_t *ab, int fd, int count )
{
    int   ret ;
    char *buff_data ;

    ret       = 0 ;
    buff_data = NULL ;

    // prepare data buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", ab->rank, count) ;
        }
    }

    // read data
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'read' %d bytes for file %d\n", ab->rank, count, fd) ;
        ret = server_files_read(fd, buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not read :-(", ab->rank) ;
        }
    }

    // send data
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, buff_data, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(", ab->rank) ;
        }
    }

    // free data buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", ab->rank) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_write ( comm_t *ab, int fd, int count )
{
    int   ret ;
    char *buff_data ;

    ret       = 0 ;
    buff_data = NULL ;

    // prepare data buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", ab->rank, count) ;
        }
    }

    // receive data
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_data, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", ab->rank) ;
        }
    }

    // write data
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'write' %d bytes for file %d\n", ab->rank, count, fd) ;
        ret = server_files_write(fd, buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not written :-(", ab->rank) ;
        }
    }

    // free data buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", ab->rank) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File System API (2)
 */

int serverstub_close2 ( comm_t *ab, int fd )
{
    buffer_t info[1] ;

    // close file
    info[0].buff        = NULL ;
    info[0].size        = 0 ;
    info[0].datatype    = MPI_CHAR ;
    info[0].remote      = fd ;
    info[0].err_msg     = "Server[%d]: close(...) fails :-(" ;
    info[0].comm_action = COM_FILE_CLOSE ;

    // Return OK/KO
    return mfs_protocol_request_do(ab, info, 4) ;
}

int serverstub_write2 ( comm_t *ab, int fd, int count )
{
    int ret ;
    buffer_t info[4] ;
    char *ptr ;

    // Prepare data buffer
    info[0].buff        = &ptr ;
    info[0].size        = count ;
    info[0].datatype    = MPI_CHAR ;
    info[0].remote      = 0 ;
    info[0].err_msg     = "Server[%d]: malloc(...) fails :-(" ;
    info[0].comm_action = COM_MALLOC ;

    // Receive data
    info[1].buff        = &ptr ;
    info[1].size        = count ;
    info[1].datatype    = MPI_CHAR ;
    info[1].remote      = MPI_ANY_SOURCE ;
    info[1].err_msg     = "Server[%d]: data not received :-(" ;
    info[1].comm_action = COM_RECV_PTRDATA_FROM ;

    // Write data
    info[2].buff        = &ptr ;
    info[2].size        = count ;
    info[2].datatype    = MPI_CHAR ;
    info[2].remote      = fd ;
    info[2].err_msg     = "Server[%d]: data not written :-(" ;
    info[2].comm_action = COM_FILE_WRITE ;

    // Free data buffer
    info[3].buff        = &ptr ;
    info[3].size        = 0 ;
    info[3].datatype    = MPI_CHAR ;
    info[3].remote      = 0 ;
    info[3].err_msg     = "Server[%d]: problem on free :-(" ;
    info[3].comm_action = COM_FREE ;

    // Do requests
    ret = mfs_protocol_request_do(ab, info, 4) ;
    if (ret < 0) {
        return -1 ;
    }

    // Return OK/KO
    return info[2].size ;
}

int serverstub_read3 ( comm_t *ab, int fd, int count )
{
    int          ret ;
    char        *buff_data ;
    struct stat  fdstat ;
    off_t        offset ;

    ret = 0 ;

    // prepare st_size and offset
    if (ret >= 0)
    {
        fstat(fd, &fdstat) ;
        offset = lseek(fd, 0L, SEEK_CUR) ;
    }

    // map buffer
    if (ret >= 0)
    {
        buff_data = (char *)server_files_mmap(NULL, fdstat.st_size, PROT_READ, MAP_SHARED, fd, 0) ;
        if (NULL == buff_data) {
            mfs_print(DBG_ERROR, "Server[%d]: mmap(%d, ... %d) fails :-(", ab->rank, fd, count) ;
	    ret = -1 ;
        }
    }

    // send data
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, buff_data+offset, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(", ab->rank) ;
        }
    }

    // unmap buffer
    if (ret >= 0)
    {
        server_files_munmap(buff_data, fdstat.st_size) ;
        lseek(fd, count, SEEK_CUR) ;
    }

    // Return OK/KO
    return ret ;
}

