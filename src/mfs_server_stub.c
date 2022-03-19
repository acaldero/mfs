
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

    // Check arguments...
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
    int     ret ;
    conf_t  conf ;
    int     local_rank ;
    char   *local_node ;

    // Initialize files
    ret = mfs_file_init() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for files :-(", -1) ;
        return -1 ;
    }

    // Initialize dbm
    ret = mfs_dbm_init() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for dbm :-(", -1) ;
        return -1 ;
    }

    // Initialize directories
    ret = mfs_directory_init() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for directories :-(", -1) ;
        return -1 ;
    }

    // Get valid configuration..
    ret = mfs_conf_get(&conf, params->conf_fname) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: mfs_conf_get('%s') fails :-(", -1, params->conf_fname) ;
        return -1 ;
    }
    if (conf.n_partitions < 1) {
        mfs_print(DBG_ERROR, "Server[%d]: mfs_conf_get fails to read at least one partition in file '%s' :-(", -1, params->conf_fname) ;
        return -1 ;
    }

    // Initialize
    ret = mfs_comm_init(wb, params->comm_backend, params, conf.active) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for comm :-(", -1) ;
        return -1 ;
    }
    if (conf.active->n_nodes != mfs_comm_get_size(wb)) {
        mfs_print(DBG_ERROR, "Server[%d]: partition in '%s' with different nodes than processes :-(", -1, params->conf_fname) ;
        return -1 ;
    }

    // Register service
    local_rank = mfs_comm_get_rank(wb) ;
    local_node = mfs_conf_get_active_node(&conf, local_rank) ;
    strcpy(wb->srv_name, local_node) ;

    ret = mfs_comm_register(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: port registration fails :-(", mfs_comm_get_rank(wb)) ;
        return -1 ;
    }

    // Free configuration
    mfs_conf_free(&conf) ; 

    // Return OK
    return 0 ;
}

int serverstub_finalize ( comm_t *wb, params_t *params )
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

    // Finalize directories
    ret = mfs_directory_finalize() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for directories :-(", -1) ;
        return -1 ;
    }

    // Finalize files
    ret = mfs_file_finalize() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for files :-(", -1) ;
        return -1 ;
    }

    // Finalize dbm
    ret = mfs_dbm_finalize() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for dbm :-(", -1) ;
        return -1 ;
    }

    // Finalize params
    //if (ret >= 0)
    {
         mfs_params_free(&params) ;
    }

    // Return OK
    return 0 ;
}

int serverstub_accept ( comm_t *ab, params_t *params, comm_t *wb )
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

int serverstub_disconnect ( comm_t *ab, params_t *params, int remote_rank )
{
    int ret ;

    // Disconnect
    ret = mfs_comm_disconnect(ab, remote_rank) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: disconnect fails :-(", mfs_comm_get_rank(ab)) ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File API
 */

int serverstub_open ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags )
{
    int   ret ;
    char *buff_data_sys ;

    // Check arguments...
    if (NULL == ab)     { return -1 ; }
    if (NULL == params) { return -1 ; }

    // Initialize...
    ret = 0 ;
    buff_data_sys  = NULL ;

    // read filename
    //if (ret >= 0)
    {
        ret = stub_read_name(ab, &buff_data_sys, params->data_prefix, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open file
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_file_open(fd, params->file_backend, buff_data_sys, flags) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file not opened :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back file descriptor
    //if (ret >= 0)
    {
	long fref = mfs_file_fd2long(*fd) ;
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: open(flags=%d) >> client\n", mfs_comm_get_rank(ab), fref, flags) ;

        ret = mfs_comm_send_data_to(ab, 0, &(fref), 1, MPI_LONG) ;
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

int serverstub_close ( comm_t *ab, params_t *params, int fd )
{
    int  ret ;

    ret = 0 ;

    // close file
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: File[%ld]: close()\n", mfs_comm_get_rank(ab), fd) ;

        ret = mfs_file_close(fd) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: close(%d) fails :-(", mfs_comm_get_rank(ab), fd) ;
        }
    }

    // send back status
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(", mfs_comm_get_rank(ab)) ;
        }
    }
    
    // Return OK/KO
    return ret ;
}

int serverstub_read ( comm_t *ab, params_t *params, int fd, int count )
{
    int    ret ;
    int    is_dynamic ;
    char  *buff_data ;
    long   buff_size ;
    char   buff_data_local[SRVSTUB_READ_BUFFLOCAL_SIZE] ;
    long   remaining_size, readed ;

    ret = 0 ;
    is_dynamic = (count < SRVSTUB_READ_UP_TO_IS_DYNAMIC) ? 1 : 0 ;
    buff_data  = NULL ;

    // prepare data buffer
    //if (ret >= 0)
    {
	if (1 == is_dynamic)
	{
            buff_size = count ;
            ret = mfs_malloc(&buff_data, count) ;
            if (ret < 0) {
                mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), count) ;
	        is_dynamic = 0 ; // if malloc fails then we try is_dynamic=off
            }
	}

	if (0 == is_dynamic) {
            buff_size = SRVSTUB_READ_BUFFLOCAL_SIZE ;
	    buff_data = buff_data_local ;
	    ret = 0 ;
	}
    }

    remaining_size = count ;
    while ( (ret >= 0) && (remaining_size > 0) )
    {
           // read data
           if (ret >= 0)
           {
               mfs_print(DBG_INFO, "Server[%d]: request 'read' %d bytes for file %d\n", mfs_comm_get_rank(ab), count, fd) ;

               ret = readed = mfs_file_read(fd, buff_data, buff_size) ;
               if (ret < 0) {
                   mfs_print(DBG_WARNING, "Server[%d]: data not read :-(", mfs_comm_get_rank(ab)) ;
               }
           }

           // send back status
           if (ret >= 0)
           {
               ret = mfs_comm_send_data_to(ab, 0, &readed, 1, MPI_INT) ;
               if (ret < 0) {
                   mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(", mfs_comm_get_rank(ab)) ;
               }
           }

           // send data
           if ( (ret >= 0) && (readed > 0) )
           {
	       mfs_print(DBG_INFO, "Server[%d]: File[%ld]: read(bytes=%d) >> client\n", mfs_comm_get_rank(ab), fd, ret) ;

               ret = mfs_comm_send_data_to(ab, 0, buff_data, readed, MPI_CHAR) ;
               if (ret < 0) {
                   mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(", mfs_comm_get_rank(ab)) ;
               }
           }

	   // if (readed == 0) then read less than count but no more data is available
	   if (0 == readed)
	        remaining_size = 0 ;
	   else remaining_size = remaining_size - readed ;
    }

    // free data buffer
    //if (ret >= 0)
    {
	if (1 == is_dynamic)
	{
            ret = mfs_free(&buff_data) ;
            if (ret < 0) {
                mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
            }
	}
    }

    // Return OK/KO
    return ret ;
}

#define MAX_BUFF_SIZE (10*1024*1024)

int serverstub_write ( comm_t *ab, params_t *params, int fd, int count )
{
    int    ret ;
    char  *buff_data ;
    int    buffer_size ;
    long   remaining_size, current_size ;

    ret       = 0 ;
    buff_data = NULL ;
    if (count > MAX_BUFF_SIZE) 
         buffer_size = MAX_BUFF_SIZE ;
    else buffer_size = count ;

    // prepare data buffer
    ret = mfs_malloc(&buff_data, buffer_size) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), count) ;
    }

    // if error then send back error and return
    if (ret < 0) {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_INT) ;
	return -1 ;
    }

    // send back buffer_size
    ret = mfs_comm_send_data_to(ab, 0, &buffer_size, 1, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buffer_size cannot be sent :-(", mfs_comm_get_rank(ab)) ;
    }

    // receive chunks
    current_size   = 0 ;
    remaining_size = count ;
  //while ( (ret > 0) && (remaining_size > 0) )
    while (remaining_size > 0)
    {
	if (remaining_size < buffer_size) {
	    buffer_size = remaining_size ;
	}

        // receive data
        //if (ret >= 0)
        {
	    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: write(bytes=%d) << client\n", mfs_comm_get_rank(ab), fd, buffer_size) ;

            ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_data, buffer_size, MPI_CHAR) ;
            if (ret < 0) {
                mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
            }
        }

        // write data
        //if (ret >= 0)
        {
            mfs_print(DBG_INFO, "Server[%d]: request 'write' %d bytes for file %d\n", mfs_comm_get_rank(ab), buffer_size, fd) ;

            ret = mfs_file_write(fd, buff_data, buffer_size) ;
            if (ret < 0) {
                mfs_print(DBG_WARNING, "Server[%d]: data not written :-(", mfs_comm_get_rank(ab)) ;
            }
        }

        current_size   = current_size   + buffer_size ;
        remaining_size = remaining_size - buffer_size ;
    }

    // send back status
    //if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, &current_size, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // free data buffer
    ret = mfs_free(&buff_data) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}


/*
 *  Directory API
 */

int serverstub_mkdir ( comm_t *ab, params_t *params, int pathname_length, int mode )
{
    long  ret ;
    char *buff_data_sys ;

    // Check arguments...
    if (NULL == ab)     { return -1 ; }
    if (NULL == params) { return -1 ; }

    // Initialize...
    ret = 0 ;
    buff_data_sys = NULL ;

    // read dirname
    //if (ret >= 0)
    {
        ret = stub_read_name(ab, &buff_data_sys, params->data_prefix, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: read_name of %d chars fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open dir
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'mkdir' for dirname %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_directory_mkdir(params->directory_backend, buff_data_sys, mode) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: dir not opened :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back status
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_LONG) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(", mfs_comm_get_rank(ab)) ;
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

int serverstub_rmdir ( comm_t *ab, params_t *params, int pathname_length )
{
    long  ret ;
    char *buff_data_sys ;

    // Check arguments...
    if (NULL == ab)     { return -1 ; }
    if (NULL == params) { return -1 ; }

    // Initialize...
    ret = 0 ;
    buff_data_sys = NULL ;

    // read dirname
    //if (ret >= 0)
    {
        ret = stub_read_name(ab, &buff_data_sys, params->data_prefix, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: read_name of %d chars fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open dir
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'mkdir' for dirname %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_directory_rmdir(params->directory_backend, buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: dir not opened :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back status
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_LONG) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(", mfs_comm_get_rank(ab)) ;
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


/*
 *  DBM File API
 */

int serverstub_dbmopen ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags )
{
    int   ret ;
    char *buff_data_sys ;

    // Check arguments...
    if (NULL == ab) { return -1 ; }
    if (NULL == fd) { return -1 ; }

    // Initialize...
    ret = 0 ;
    buff_data_sys  = NULL ;

    // read filename
    //if (ret >= 0)
    {
        ret = stub_read_name(ab, &buff_data_sys, params->data_prefix, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open file
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_dbm_open(fd, params->dbm_backend, buff_data_sys, flags) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file not opened :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back file descriptor
    //if (ret >= 0)
    {
	long fref = mfs_dbm_fd2long(*fd) ;
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: open(flags=%d) >> client\n", mfs_comm_get_rank(ab), fref, flags) ;

        ret = mfs_comm_send_data_to(ab, 0, &(fref), 1, MPI_LONG) ;
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

int serverstub_dbmclose ( comm_t *ab, params_t *params, int fd )
{
    int  ret ;

    ret = 0 ;

    // close file
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: File[%ld]: close()\n", mfs_comm_get_rank(ab), fd) ;

        ret = mfs_dbm_close(fd) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: close(%d) fails :-(", mfs_comm_get_rank(ab), fd) ;
        }
    }

    // send back status
    //if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(", mfs_comm_get_rank(ab)) ;
        }
    }
    
    // Return OK/KO
    return ret ;
}

int serverstub_dbmstore ( comm_t *ab, params_t *params, int fd, int count )
{
    int    ret ;
    char  *buff_key, *buff_val ;
    int    key_size,  val_size ;

    ret      = 0 ;
    key_size = count ;

    // (1) receive val_size
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, &val_size, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // (2) prepare key buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), key_size) ;
        }
    }

    // prepare val buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_val, val_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), val_size) ;
        }
    }

    // (3) if error then send back error and return
    if (ret < 0) {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_INT) ;
        mfs_print(DBG_WARNING, "Server[%d]: dbmstore(status:%d) >> client", mfs_comm_get_rank(ab), ret) ;
	return -1 ;
    }

    // send back status for malloc...
    ret = mfs_comm_send_data_to(ab, 0, &val_size, 1, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buff_val cannot be sent :-(", mfs_comm_get_rank(ab)) ;
    }

    // (4) receive key
    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmstore(bytes=%d) << client\n", mfs_comm_get_rank(ab), fd, key_size) ;
    ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_key, key_size, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
    }

    // (5) receive val
    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmstore(bytes=%d) << client\n", mfs_comm_get_rank(ab), fd, val_size) ;
    ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_val, val_size, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
    }

    // (6) do dbm_store...
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmstore(key=%.*s, val=%.*s) >> client\n", mfs_comm_get_rank(ab), fd, key_size, buff_key, val_size, buff_val) ;

        ret = mfs_dbm_store(fd, buff_key, key_size, buff_val, val_size) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on mfs_dbm_store :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // (7) send back status for val
    //if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // (8) free data buffer
    ret = mfs_free(&buff_key) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
    }

    ret = mfs_free(&buff_val) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}

int serverstub_dbmfetch ( comm_t *ab, params_t *params, int fd, int count )
{
    int    ret, status ;
    char  *buff_key, *buff_val ;
    int    key_size,  val_size ;
    long   remaining_size, current_size ;

    ret      = 0 ;
    key_size = count ;

    // (1) receive val_size
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, &val_size, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // (2) prepare key buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), key_size) ;
        }
    }

    // prepare val buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_val, val_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), val_size) ;
        }
    }

    // (3) if error then send back error and return
    if (ret < 0) {
        ret = mfs_comm_send_data_to(ab, 0, &ret, 1, MPI_INT) ;
        mfs_print(DBG_WARNING, "Server[%d]: dbmfetch(status:%d) >> client", mfs_comm_get_rank(ab), ret) ;
	return -1 ;
    }

    // send back status for malloc...
    ret = mfs_comm_send_data_to(ab, 0, &val_size, 1, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buff_val cannot be sent :-(", mfs_comm_get_rank(ab)) ;
    }

    // (4) receive key
    ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_key, key_size, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
    }

    // (5) do dbm_fetch ...
    if (ret >= 0)
    {
        ret = status = mfs_dbm_fetch(fd, buff_key, key_size, (void **)&buff_val, &val_size) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on mfs_dbm_fetch :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // (6) send back status for malloc...
    ret = mfs_comm_send_data_to(ab, 0, &status, 1, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buff_val cannot be sent :-(", mfs_comm_get_rank(ab)) ;
    }
    mfs_print(DBG_INFO, "Server[%d]: dbmfetch(status:%d) >> client", mfs_comm_get_rank(ab), status) ;

    // (7) send val
    if (status >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmfetch(buff_val=%.*s of %d bytes) >> client\n", mfs_comm_get_rank(ab), fd, val_size, buff_val, val_size) ;

        ret = mfs_comm_send_data_to(ab, 0, buff_val, val_size, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // (8) free data buffer
    ret = mfs_free(&buff_key) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
    }

    ret = mfs_free(&buff_val) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}

int serverstub_dbmdelete ( comm_t *ab, params_t *params, int fd, int count )
{
    int    ret, status ;
    char  *buff_key, *buff_val ;
    int    key_size,  val_size ;
    long   remaining_size, current_size ;

    ret      = 0 ;
    key_size = count ;

    // (1) prepare key buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(", mfs_comm_get_rank(ab), key_size) ;
        }
    }

    // (2) receive key
    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmdelete(bytes=%d) << client\n", mfs_comm_get_rank(ab), fd, key_size) ;
    ret = mfs_comm_recv_data_from(ab, MPI_ANY_SOURCE, buff_key, key_size, MPI_CHAR) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(", mfs_comm_get_rank(ab)) ;
    }

    // (3) do dbm_delete ...
    if (ret >= 0)
    {
        ret = status = mfs_dbm_delete(fd, buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on mfs_dbm_delete with given key :-(", mfs_comm_get_rank(ab)) ;
        }
    }

    // (4) send back status for malloc...
    ret = mfs_comm_send_data_to(ab, 0, &status, 1, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buff_val cannot be sent :-(", mfs_comm_get_rank(ab)) ;
    }

    // (5) free data buffer
    ret = mfs_free(&buff_key) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}

