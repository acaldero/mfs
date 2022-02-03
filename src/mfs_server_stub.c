
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
 *  Auxiliar (internal) functions
 */

int stub_read_name ( comm_t *ab, char **buff_data_sys, char *base_dirname, int pathname_length )
{
    int ret ;
    char *buff_data_user ;

    // Check params...
    if (NULL == ab)            { return -1 ; }
    if (NULL == buff_data_sys) { return -1 ; }

    // Initialize...
    ret = 0 ;
    (*buff_data_sys) = NULL ;
      buff_data_user = NULL ;

    // prepare name buffer
    //if (ret >= 0)
    {
        ret = mfs_malloc(&buff_data_user, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // read name
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: request for a name of %d chars\n", mfs_comm_get_rank(ab), pathname_length) ;

        ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_data_user, pathname_length, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: name not received :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // from user dirname to internal dirname
    if (ret >= 0)
    {
        int buff_data_len = strlen(base_dirname) + pathname_length + 16 ;  // base_dirname_value + '/' + pathname + '.<rank>'

	ret = mfs_malloc(buff_data_sys, buff_data_len) ;
	if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), buff_data_len) ;
        }
	else {
	    sprintf((*buff_data_sys), "%s/%s", base_dirname, buff_data_user) ;
	}
    }

    // free name buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_user) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  Server stub API
 */

int serverstub_init ( comm_t *wb, params_t *params )
{
    int ret ;

    // Initialize
    ret = mfs_comm_init(wb, params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails :-(", -1) ;
        return -1 ;
    }

    // Register service
    sprintf(wb->srv_name, "%s.%d", MFS_SERVER_STUB_PNAME, mfs_comm_get_rank(wb)) ;

    ret = mfs_comm_register(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: port registration fails :-(", mfs_comm_get_rank(wb)) ;
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
        mfs_print(DBG_ERROR, "Server[%d]: port unregistration fails :-(", mfs_comm_get_rank(wb)) ;
        return -1 ;
    }

    // Finalize
    ret = mfs_comm_finalize(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails :-(", mfs_comm_get_rank(wb)) ;
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
        mfs_print(DBG_ERROR, "Server[%d]: accept connections fails :-(", mfs_comm_get_rank(ab)) ;
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
        mfs_print(DBG_ERROR, "Server[%d]: disconnect fails :-(", mfs_comm_get_rank(ab)) ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File System API
 */

int serverstub_open ( comm_t *ab, file_t *fd, char *base_dirname, int pathname_length, int flags )
{
    int   ret ;
    char *buff_data_sys ;

    // Check params...
    if (NULL == ab) { return -1 ; }
    if (NULL == fd) { return -1 ; }

    // Initialize...
    ret = 0 ;
    buff_data_sys  = NULL ;

    // read filename
    //if (ret >= 0)
    {
        ret = stub_read_name(ab, &buff_data_sys, base_dirname, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open file
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_file_open(fd, fd->file_protocol, buff_data_sys, flags) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file not opened :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back file descriptor
    if (ret >= 0)
    {
	long fh = mfs_file_fd2long(fd) ;
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: open(flags=%d) >> client\n", mfs_comm_get_rank(ab), fh, flags) ;

        ret = mfs_comm_send_data_to(ab, 0, &fh, 1, MPI_LONG) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file descriptor cannot be sent :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // free filename buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_close ( comm_t *ab, file_t *fd )
{
    int  ret ;

    ret = 0 ;

    // close file
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: File[%ld]: close()\n", mfs_comm_get_rank(ab), mfs_file_fd2long(fd)) ;

        ret = mfs_file_close(fd) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: close(%d) fails :-(", mfs_comm_get_rank(ab), mfs_file_fd2long(fd)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_read ( comm_t *ab, file_t *fd, int count )
{
    int    ret ;
    char  *buff_data ;

    ret       = 0 ;
    buff_data = NULL ;

    // prepare data buffer
    //if (ret >= 0)
    {
        ret = mfs_malloc(&buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), count) ;
        }
    }

    // read data
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'read' %d bytes for file %d\n", mfs_comm_get_rank(ab), count, mfs_file_fd2long(fd)) ;

        ret = mfs_file_read(fd, buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not read :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // send data
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: File[%ld]: read(bytes=%d) >> client\n", mfs_comm_get_rank(ab), mfs_file_fd2long(fd), count) ;

        ret = mfs_comm_send_data_to(ab, 0, buff_data, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // free data buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_write ( comm_t *ab, file_t *fd, int count )
{
    int    ret ;
    char  *buff_data ;

    ret       = 0 ;
    buff_data = NULL ;

    // prepare data buffer
    //if (ret >= 0)
    {
        ret = mfs_malloc(&buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), count) ;
        }
    }

    // receive data
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: File[%ld]: write(bytes=%d) << client\n", mfs_comm_get_rank(ab), mfs_file_fd2long(fd), count) ;

        ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_data, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // write data
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'write' %d bytes for file %d\n", mfs_comm_get_rank(ab), count, mfs_file_fd2long(fd)) ;

        ret = mfs_file_write(fd, buff_data, count) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not written :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // free data buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File System API (2)
 */

int serverstub_read2 ( comm_t *ab, long fd, int count )
{
    int          ret ;
    char        *buff_data ;
    struct stat  fdstat ;
    off_t        offset ;
    long         to_read ;

    ret = 0 ;

    // prepare st_size and offset
    if (ret >= 0)
    {
        fstat(fd, &fdstat) ;
        offset = lseek(fd, 0L, SEEK_CUR) ;

	to_read = fdstat.st_size - offset ;
	to_read = (to_read < count) ? to_read : count ;
    }

    // map buffer
    if (ret >= 0)
    {
        buff_data = (char *)mfs_file_mmap(NULL, fdstat.st_size, PROT_READ, MAP_SHARED, fd, 0) ;
        if (NULL == buff_data) {
            mfs_print(DBG_ERROR, "Server[%d]: mmap(%d, ... %d) fails :-(", mfs_comm_get_rank(ab), fd, count) ;
	    ret = -1 ;
        }
    }

    // send data
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, buff_data+offset, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // unmap buffer
    if (ret >= 0)
    {
        mfs_file_munmap(buff_data, fdstat.st_size) ;
        lseek(fd, to_read, SEEK_CUR) ;
    }

    // TODO: send to_read, so client knows the readed bytes (could be less that count bytes). Same on read1.

    // Return OK/KO
    return ret ;
}


/*
 *  Directory API
 */

int serverstub_mkdir ( comm_t *ab, char *base_dirname, int pathname_length, int mode )
{
    int   ret ;
    char *buff_data_sys ;

    // Check params...
    if (NULL == ab)           { return -1 ; }
    if (NULL == base_dirname) { return -1 ; }

    // Initialize...
    ret = 0 ;
    buff_data_sys = NULL ;

    // read dirname
    //if (ret >= 0)
    {
        ret = stub_read_name(ab, &buff_data_sys, base_dirname, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: read_name of %d chars fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open dir
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'mkdir' for dirname %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_directory_mkdir(buff_data_sys, mode) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: dir not opened :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // free dirname buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_rmdir ( comm_t *ab, char *base_dirname, int pathname_length )
{
    int   ret ;
    char *buff_data_sys ;

    // Check params...
    if (NULL == ab)           { return -1 ; }
    if (NULL == base_dirname) { return -1 ; }

    // Initialize...
    ret = 0 ;
    buff_data_sys = NULL ;

    // read dirname
    //if (ret >= 0)
    {
        ret = stub_read_name(ab, &buff_data_sys, base_dirname, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: read_name of %d chars fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open dir
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'mkdir' for dirname %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_directory_rmdir(buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: dir not opened :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // free dirname buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

