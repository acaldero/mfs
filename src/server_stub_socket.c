
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of XPNlite.
 *
 *  XPNlite is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPNlite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with XPNlite.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "server_stub_socket.h"


/*
 *  Auxiliar (internal) functions
 */

int stub_read_name ( comm_t *ab, char **buff_data_sys, int pathname_length )
{
    int ret ;
    char *buff_data_user ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab,            "[SERV_STUB] NULL ab             :-/", -1) ;
    NULL_PRT_MSG_RET_VAL(buff_data_sys, "[SERV_STUB] NULL buff_data_sys  :-/", -1) ;

    // Initialize...
    ret = 0 ;
    buff_data_user = NULL ;

    // prepare name buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_data_user, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // read name
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: request for a name of %d chars\n", mfs_comm_get_rank(ab), pathname_length) ;

        ret = mfs_comm_socket_recv_data_from(ab, MPI_ANY_SOURCE, buff_data_user, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: name not received :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // from user dirname to internal dirname
    if (ret >= 0)
    {
	strcat((*buff_data_sys), buff_data_user) ;
    }

    // free name buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_user) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  Server stub API
 */

int serverstub_socket_init ( comm_t *wb, params_t *params, int size, int rank )
{
    int    ret, rr_node ;
    conf_t partitions ;

    // Initialize files
    ret = mfs_file_init() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for files :-(\n", -1) ;
        return -1 ;
    }

    // Initialize dbm
    ret = mfs_dbm_init() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for dbm :-(\n", -1) ;
        return -1 ;
    }

    // Initialize directories
    ret = mfs_directory_init() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for directories :-(\n", -1) ;
        return -1 ;
    }

    // Initial default values
    mfs_comm_reset(wb) ;
    wb->comm_protocol      = COMM_USE_SOCKET ;
    wb->comm_protocol_name = "SOCKET" ;
    wb->ns_backend         = params->ns_backend ;

    // Get valid configuration..
    ret = info_fsconf_get(&partitions, params->conf_fname) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: info_fsconf_get('%s') fails :-(\n", -1, params->conf_fname) ;
        return -1 ;
    }
    if (partitions.n_partitions < 1) {
        mfs_print(DBG_ERROR, "Server[%d]: info_fsconf_get fails to read at least one partition in file '%s' :-(\n", -1, params->conf_fname) ;
        return -1 ;
    }

    // Initialize (fields and server socket)
    wb->rank = rank ;
    wb->size = size ;
    rr_node  = (wb->rank) % info_fsconf_get_active_nnodes(&partitions) ;
    wb->node_str = info_fsconf_get_active_node(&partitions, rr_node) ;
    wb->node_url = info_fsconf_get_active_url (&partitions, rr_node) ;
    wb->node_str = base_strdup(wb->node_str) ;
    wb->node_url = base_url_dup(wb->node_url) ;

    ret = base_socket_serversocket(&(wb->sd), wb->node_url->port) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: initialization fails for base_socket_serversocket :-(\n", -1) ;
        return -1 ;
    }

    // Register service
    ret = info_ns_get_portname(wb->port_name, wb->sd) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: info_ns_get_portname fails :-(\n", mfs_comm_get_rank(wb)) ;
        return -1 ;
    }

    ret = info_ns_insert(wb, wb->ns_backend, wb->node_str, wb->port_name) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: port registration fails :-(\n", mfs_comm_get_rank(wb)) ;
        return -1 ;
    }

    mfs_print(DBG_INFO, "Server[%d]: ns_insert(key:%s, val:%s)\n", mfs_comm_get_rank(wb), wb->node_str, wb->port_name) ;

    // Free configuration
    ret = info_fsconf_free(&partitions) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for info_fsconf_free :-(\n", -1) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_socket_finalize ( comm_t *wb, params_t *params )
{
    int  ret ;

    // UnRegister service
    ret = info_ns_remove(wb, wb->ns_backend, wb->node_str) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: port unregistration fails :-(\n", mfs_comm_get_rank(wb)) ;
        return -1 ;
    }

    // Finalize
    ret = base_socket_close(&(wb->sd)) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails :-(\n", mfs_comm_get_rank(wb)) ;
        return -1 ;
    }

    wb->is_connected = 0 ;

    // Finalize directories
    ret = mfs_directory_finalize() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for directories :-(\n", -1) ;
        return -1 ;
    }

    // Finalize files
    ret = mfs_file_finalize() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for files :-(\n", -1) ;
        return -1 ;
    }

    // Finalize dbm
    ret = mfs_dbm_finalize() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for dbm :-(\n", -1) ;
        return -1 ;
    }

    // Free memory
    ret = mfs_free(&(wb->node_str)) ;
    ret = base_url_free(&(wb->node_url)) ;

    // Finalize params
    ret = info_params_free(params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails for params_free :-(\n", -1) ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_socket_accept ( comm_t *ab, params_t *params, comm_t *wb )
{
    int ret ;

    // *ab = *wb ;
    memmove(ab, wb, sizeof(comm_t)) ;

    // Accept connections...
    ab->dd = base_socket_accept(ab->sd) ;
    if (ab->dd < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: accept connections fails :-(\n", mfs_comm_get_rank(ab)) ;
        return -1 ;
    }

    ab->is_connected = 1 ;

    // Return OK
    return 0 ;
}

int serverstub_socket_disconnect_all ( comm_t *ab, params_t *params )
{
    int ret ;

    // Disconnect
    ret = base_socket_close(&(ab->dd)) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: disconnect %d fails :-(\n", mfs_comm_get_rank(ab), ab->dd) ;
    }

    ab->is_connected = 0 ;

    // Return OK/KO
    return ret ;
}


/*
 *  File API
 */

int serverstub_socket_open ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags )
{
    long    ret ;
    char   *buff_data_sys ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab,     "[SERV_STUB] NULL ab      :-/", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[SERV_STUB] NULL params  :-/", -1) ;

    // Initialize...
    ret = 0 ;
    buff_data_sys  = NULL ;

    // read filename
    if (ret >= 0)
    {
        ret = base_str_prepare_pathname(&buff_data_sys, ab->node_url->file, ab->local_rank, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: base_str_prepare_pathname(%d) fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }

        ret = stub_read_name(ab, &buff_data_sys, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: stub_read_name(%d) fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open file
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_file_open(fd, params->file_backend, buff_data_sys, flags) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file '%s' not opened :-(\n", mfs_comm_get_rank(ab), buff_data_sys) ;
        }
    }

    // send back file descriptor
    //if (ret >= 0)
    {
	long fref = mfs_file_fd2long(*fd) ;
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: open(flags=%d) >> client\n", mfs_comm_get_rank(ab), fref, flags) ;

        ret = mfs_comm_socket_send_data_to(ab, 0, &(fref), sizeof(long)) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file descriptor cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // free filename buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_close ( comm_t *ab, params_t *params, int fd )
{
    int  ret ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab :-/", -1) ;

    // Initialize...
    ret = 0 ;

    // close file
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: File[%ld]: close()\n", mfs_comm_get_rank(ab), fd) ;

        ret = mfs_file_close(fd) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: close(%d) fails :-(\n", mfs_comm_get_rank(ab), fd) ;
        }
    }

    // send back status
    if (ret >= 0)
    {
        ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(int)) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_read ( comm_t *ab, params_t *params, int fd, int count )
{
    int    ret ;
    int    is_dynamic ;
    char  *buff_data ;
    long   buff_size ;
    char   buff_data_local[SRVSTUB_READ_BUFFLOCAL_SIZE] ;
    long   remaining_size, readed ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab :-/", -1) ;

    // Initialize...
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
                mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), count) ;
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
                   mfs_print(DBG_WARNING, "Server[%d]: data not read :-(\n", mfs_comm_get_rank(ab)) ;
               }
           }

           // send back status
           if (ret >= 0)
           {
               ret = mfs_comm_socket_send_data_to(ab, 0, &readed, sizeof(int)) ;
               if (ret < 0) {
                   mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
               }
           }

           // send data
           if ( (ret >= 0) && (readed > 0) )
           {
	       mfs_print(DBG_INFO, "Server[%d]: File[%ld]: read(bytes=%d) >> client\n", mfs_comm_get_rank(ab), fd, ret) ;

               ret = mfs_comm_socket_send_data_to(ab, 0, buff_data, readed) ;
               if (ret < 0) {
                   mfs_print(DBG_WARNING, "Server[%d]: data cannot be sent fails :-(\n", mfs_comm_get_rank(ab)) ;
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
                mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
            }
	}
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_write ( comm_t *ab, params_t *params, int fd, int count )
{
    int    ret ;
    char  *buff_data ;
    int    buffer_size ;
    long   remaining_size, current_size ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab :-/", -1) ;

    // Initialize...
    ret       = 0 ;
    buff_data = NULL ;
    if (count > SRVSTUB_WRITE_MAXBUFF_SIZE)
         buffer_size = SRVSTUB_WRITE_MAXBUFF_SIZE ;
    else buffer_size = count ;

    // prepare data buffer
    ret = mfs_malloc(&buff_data, buffer_size) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), count) ;
    }

    // if error then send back error and return
    if (ret < 0) {
        ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(int)) ;
	return -1 ;
    }

    // send back buffer_size
    ret = mfs_comm_socket_send_data_to(ab, 0, &buffer_size, sizeof(int)) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buffer_size cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
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

            ret = mfs_comm_socket_recv_data_from(ab, MPI_ANY_SOURCE, buff_data, buffer_size) ;
            if (ret < 0) {
                mfs_print(DBG_WARNING, "Server[%d]: data not received :-(\n", mfs_comm_get_rank(ab)) ;
            }
        }

        // write data
        //if (ret >= 0)
        {
            mfs_print(DBG_INFO, "Server[%d]: request 'write' %d bytes for file %d\n", mfs_comm_get_rank(ab), buffer_size, fd) ;

            ret = mfs_file_write(fd, buff_data, buffer_size) ;
            if (ret < 0) {
                mfs_print(DBG_WARNING, "Server[%d]: data not written :-(\n", mfs_comm_get_rank(ab)) ;
            }
        }

        current_size   = current_size   + buffer_size ;
        remaining_size = remaining_size - buffer_size ;
    }

    // send back status
    //if (ret >= 0)
    {
        ret = mfs_comm_socket_send_data_to(ab, 0, &current_size, sizeof(int)) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // free data buffer
    ret = mfs_free(&buff_data) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}


/*
 *  Directory API
 */

int serverstub_socket_mkdir ( comm_t *ab, params_t *params, int pathname_length, int mode )
{
    long    ret ;
    char   *buff_data_sys ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab,     "[SERV_STUB] NULL ab      :-/", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[SERV_STUB] NULL params  :-/", -1) ;

    // Initialize...
    ret = 0 ;
    buff_data_sys = NULL ;

    // read dirname
    if (ret >= 0)
    {
        ret = base_str_prepare_pathname(&buff_data_sys, ab->node_url->file, ab->local_rank, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: base_str_prepare_pathname(%d) fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }

        ret = stub_read_name(ab, &buff_data_sys, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: read_name of %d chars fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open dir
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'mkdir' for dirname %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_directory_mkdir(params->directory_backend, buff_data_sys, mode) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: dir not opened :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back status
    if (ret >= 0)
    {
        ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(long)) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // free dirname buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_rmdir ( comm_t *ab, params_t *params, int pathname_length )
{
    long    ret ;
    char   *buff_data_sys ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab,     "[SERV_STUB] NULL ab      :-/", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[SERV_STUB] NULL params  :-/", -1) ;

    // Initialize...
    ret = 0 ;
    buff_data_sys = NULL ;

    // read dirname
    //if (ret >= 0)
    {
        ret = base_str_prepare_pathname(&buff_data_sys, ab->node_url->file, ab->local_rank, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: base_str_prepare_pathname(%d) fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }

        ret = stub_read_name(ab, &buff_data_sys, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: read_name of %d chars fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open dir
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'mkdir' for dirname %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_directory_rmdir(params->directory_backend, buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: dir not opened :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back status
    if (ret >= 0)
    {
        ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(long)) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // free dirname buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  DBM File API
 */

int serverstub_socket_dbmopen ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags )
{
    long    ret ;
    char   *buff_data_sys ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab  :-/", -1) ;
    NULL_PRT_MSG_RET_VAL(fd, "[SERV_STUB] NULL fd  :-/", -1) ;

    // Initialize...
    ret = 0 ;
    buff_data_sys  = NULL ;

    // read filename
    if (ret >= 0)
    {
        ret = base_str_prepare_pathname(&buff_data_sys, ab->node_url->file, ab->local_rank, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: base_str_prepare_pathname(%d) fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }

        ret = stub_read_name(ab, &buff_data_sys, pathname_length) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: stub_read_name(%d) fails :-(\n", mfs_comm_get_rank(ab), pathname_length) ;
        }
    }

    // open file
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: request 'open' for filename %s\n", mfs_comm_get_rank(ab), buff_data_sys) ;

	ret = mfs_dbm_open(fd, params->dbm_backend, buff_data_sys, flags) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file not opened :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // send back file descriptor
    //if (ret >= 0)
    {
	long fref = mfs_dbm_fd2long(*fd) ;
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: open(flags=%d) >> client\n", mfs_comm_get_rank(ab), fref, flags) ;

        ret = mfs_comm_socket_send_data_to(ab, 0, &(fref), sizeof(long)) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: file descriptor cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // free filename buffer
    //if (ret >= 0)
    {
        ret = mfs_free(&buff_data_sys) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_dbmclose ( comm_t *ab, params_t *params, int fd )
{
    int ret ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab  :-/", -1) ;

    // Initialize variables
    ret = 0 ;

    // close file
    if (ret >= 0)
    {
	mfs_print(DBG_INFO, "Server[%d]: File[%ld]: close()\n", mfs_comm_get_rank(ab), fd) ;

        ret = mfs_dbm_close(fd) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: close(%d) fails :-(\n", mfs_comm_get_rank(ab), fd) ;
        }
    }

    // send back status
    ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(int)) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_dbmstore ( comm_t *ab, params_t *params, int fd, int key_size, int val_size )
{
    int    ret ;
    char  *buff_key, *buff_val ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab  :-/", -1) ;

    // (1) Initialize variables
    ret = 0 ;

    // prepare key buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), key_size) ;
        }
    }

    // prepare val buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_val, val_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), val_size) ;
        }
    }

    // (2) if error then send back error and return
    if (ret < 0) {
        ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(int)) ;
        mfs_print(DBG_WARNING, "Server[%d]: dbmstore(status:%d) >> client\n", mfs_comm_get_rank(ab), ret) ;
	return -1 ;
    }

    // send back status for malloc...
    ret = mfs_comm_socket_send_data_to(ab, 0, &val_size, sizeof(int)) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buff_val cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (3) receive key
    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmstore(bytes=%d) << client\n", mfs_comm_get_rank(ab), fd, key_size) ;
    ret = mfs_comm_socket_recv_data_from(ab, MPI_ANY_SOURCE, buff_key, key_size) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (4) receive val
    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmstore(bytes=%d) << client\n", mfs_comm_get_rank(ab), fd, val_size) ;
    ret = mfs_comm_socket_recv_data_from(ab, MPI_ANY_SOURCE, buff_val, val_size) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (5) do dbm_store...
    if (ret >= 0)
    {
        mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmstore(key=%.*s, val=%.*s) >> client\n", mfs_comm_get_rank(ab), fd, key_size, buff_key, val_size, buff_val) ;

        ret = mfs_dbm_store(fd, buff_key, key_size, buff_val, val_size) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on mfs_dbm_store :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // (6) send back status for val
    ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(int)) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: operation status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (8) free data buffer
    ret = mfs_free(&buff_key) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
    }

    ret = mfs_free(&buff_val) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_dbmfetch ( comm_t *ab, params_t *params, int fd, int key_size, int val_size )
{
    int    ret, status ;
    char  *buff_key, *buff_val ;
    long   remaining_size, current_size ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab  :-/", -1) ;

    // (1) Initialize variables
    ret = 0 ;

    // prepare key buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), key_size) ;
        }
    }

    // prepare val buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_val, val_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), val_size) ;
        }
    }

    // (2) if error then send back error and return
    if (ret < 0) {
        ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(int)) ;
        mfs_print(DBG_WARNING, "Server[%d]: dbmfetch(status:%d) >> client\n", mfs_comm_get_rank(ab), ret) ;
	return -1 ;
    }

    // send back status for malloc...
    ret = mfs_comm_socket_send_data_to(ab, 0, &val_size, sizeof(int)) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: buff_val cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (3) receive key
    ret = mfs_comm_socket_recv_data_from(ab, MPI_ANY_SOURCE, buff_key, key_size) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // do dbm_fetch ...
    if (ret >= 0)
    {
        ret = status = mfs_dbm_fetch(fd, buff_key, key_size, (void **)&buff_val, &val_size) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on mfs_dbm_fetch :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // (4) send back status for ko or val_size for ok
    if (ret < 0)
    {
        ret = mfs_comm_socket_send_data_to(ab, 0, &ret, sizeof(int)) ;
        mfs_print(DBG_WARNING, "Server[%d]: dbmfetch(status:%d) >> client\n", mfs_comm_get_rank(ab), ret) ;
        return -1 ;
    }

    ret = mfs_comm_socket_send_data_to(ab, 0, &val_size, sizeof(int)) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: val_size cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (5) send val
    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmfetch(buff_val=%.*s of %d bytes) >> client\n", mfs_comm_get_rank(ab), fd, val_size, buff_val, val_size) ;

    ret = mfs_comm_socket_send_data_to(ab, 0, buff_val, val_size) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (6) free data buffer
    ret = mfs_free(&buff_key) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
    }

    ret = mfs_free(&buff_val) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on free :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}

int serverstub_socket_dbmdelete ( comm_t *ab, params_t *params, int fd, int key_size )
{
    int    ret, status ;
    char  *buff_key, *buff_val ;
    int    val_size ;
    long   remaining_size, current_size ;

    // Check arguments...
    NULL_PRT_MSG_RET_VAL(ab, "[SERV_STUB] NULL ab  :-/", -1) ;

    // Initialize variables
    ret = 0 ;

    // (1) prepare key buffer
    if (ret >= 0)
    {
        ret = mfs_malloc(&buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: malloc(%d) fails :-(\n", mfs_comm_get_rank(ab), key_size) ;
        }
    }

    // (2) receive key
    mfs_print(DBG_INFO, "Server[%d]: File[%ld]: dbmdelete(bytes=%d) << client\n", mfs_comm_get_rank(ab), fd, key_size) ;
    ret = mfs_comm_socket_recv_data_from(ab, MPI_ANY_SOURCE, buff_key, key_size) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: data not received :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (3) do dbm_delete ...
    if (ret >= 0)
    {
        ret = status = mfs_dbm_delete(fd, buff_key, key_size) ;
        if (ret < 0) {
            mfs_print(DBG_WARNING, "Server[%d]: problem on mfs_dbm_delete with given key :-(\n", mfs_comm_get_rank(ab)) ;
        }
    }

    // (4) send back status...
    ret = mfs_comm_socket_send_data_to(ab, 0, &status, sizeof(int)) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: status cannot be sent :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // (5) free data buffer
    ret = mfs_free(&buff_key) ;
    if (ret < 0) {
        mfs_print(DBG_WARNING, "Server[%d]: problem on mfs_free :-(\n", mfs_comm_get_rank(ab)) ;
    }

    // Return OK/KO
    return ret ;
}

