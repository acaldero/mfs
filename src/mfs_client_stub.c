
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


/*
 *  File System API
 */

int clientstub_init ( comm_t *wb, params_t *params )
{
    int ret = 0 ;
    int remote_rank = 0 ;

    // Initialize
    if (ret >= 0)
    {
        ret = mfs_comm_init(wb, COMM_USE_MPI, params) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: initialization fails :-(", -1) ;
        }
    }

    // Connect to service
    if (ret >= 0)
    {
        remote_rank = (mfs_comm_get_rank(wb) % wb->n_servers) ;
        sprintf(wb->srv_name, "%s.%d", MFS_SERVER_STUB_PNAME, remote_rank) ;

        ret = mfs_comm_connect(wb, remote_rank) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: connection fails :-(", remote_rank) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int clientstub_finalize ( comm_t *wb )
{
    int ret = 0 ;

    // Remote disconnect...
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_DISCONNECT, 0, 0) ;
    }

    // Disconnect...
    if (ret >= 0)
    {
        int remote_rank = (mfs_comm_get_rank(wb) % wb->n_servers) ;

        ret = mfs_comm_disconnect(wb, remote_rank) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: disconnect fails :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Finalize
    if (ret >= 0)
    {
        ret = mfs_comm_finalize(wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: finalization fails :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File API
 */

long clientstub_open ( comm_t *wb, const char *pathname, int flags )
{
    int  ret = 0 ;
    long fd  = -1 ;

    // Send open msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_OPEN, strlen(pathname) + 1, flags) ;
    }

    // Send pathname
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(wb, 0, (void *)pathname, strlen(pathname) + 1, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Receive descriptor
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &fd, 1, MPI_LONG) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: file descriptor not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return file descriptor
    return fd ;
}

int clientstub_close ( comm_t *wb, long fd )
{
    int ret = 0 ;
    int status ;

    // Send close msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_CLOSE, fd, 0) ;
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int clientstub_read ( comm_t *wb, long fd, void *buff_char, int count )
{
    int  ret = 0 ;
    int  status ;
    long remaining_size, current_size ;

    // Send read msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_READ, fd, count) ;
    }

    current_size   = 0 ;
    remaining_size = count ;
    while ( (ret >= 0) && (remaining_size > 0) )
    {
          // Receive status
          if (ret >= 0)
          {
              ret = mfs_comm_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
              if (ret < 0) {
                  mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(", mfs_comm_get_rank(wb)) ;
              }
          }

          // Receive data
          if ( (ret >= 0) && (status > 0) )
          {
              ret = mfs_comm_recv_data_from(wb, 0, buff_char + current_size, status, MPI_CHAR) ;
              if (ret < 0) {
    	          mfs_print(DBG_ERROR, "Client[%d]: data not received :-(", mfs_comm_get_rank(wb)) ;
              }
          }

	  current_size   = current_size   + status ;
          remaining_size = remaining_size - status ;

	  // if (status == 0) then read less than count but no more data is available
          if (0 == status) {
              remaining_size = 0 ;
          }
    }

    // Return bytes read
    return current_size ;
}

int clientstub_write ( comm_t *wb, long fd, void *buff_char, int count )
{
    int  ret = 0 ;
    int  status, buffer_size ;
    long remaining_size, current_size ;

    // Send write msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_WRITE, fd, count) ;
    }

    // Receive buffer_size
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &buffer_size, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: buffer_size not received :-(", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
	if (buffer_size < 0) {
	    return -1 ;
	}
    }

    current_size   = 0 ;
    remaining_size = count ;
    while ( (ret > 0) && (remaining_size > 0) )
    {
        // Send data
        if (ret >= 0)
        {
            ret = mfs_comm_send_data_to(wb, 0, buff_char + current_size, buffer_size, MPI_CHAR) ;
            if (ret < 0) {
                mfs_print(DBG_ERROR, "Client[%d]: data cannot be sent :-(", mfs_comm_get_rank(wb)) ;
            }
        }

        current_size   = current_size   + buffer_size ;
        remaining_size = remaining_size - buffer_size ;
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return bytes written
    return status ;
}


/*
 *  Directory API
 */

long clientstub_mkdir ( comm_t *wb, const char *pathname, int mode )
{
    int ret = 0 ;
    int status ;

    // Send open msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_MKDIR, strlen(pathname) + 1, mode) ;
    }

    // Send pathname
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(wb, 0, (void *)pathname, strlen(pathname) + 1, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return status
    return status ;
}

long clientstub_rmdir ( comm_t *wb, const char *pathname )
{
    int ret = 0 ;
    int status ;

    // Send open msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_RMDIR, strlen(pathname) + 1, 0) ;
    }

    // Send pathname
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(wb, 0, (void *)pathname, strlen(pathname) + 1, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return status
    return status ;
}


/*
 *  DBM File API
 */

long clientstub_dbmopen ( comm_t *wb, const char *pathname, int flags )
{
    int  ret = 0 ;
    long fd  = -1 ;

    // Send open msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_DBMOPEN, strlen(pathname) + 1, flags) ;
    }

    // Send pathname
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(wb, 0, (void *)pathname, strlen(pathname) + 1, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Receive descriptor
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &fd, 1, MPI_LONG) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: file descriptor not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return file descriptor
    return fd ;
}

int  clientstub_dbmclose ( comm_t *wb, long fd )
{
    int ret = 0 ;
    int status ;

    // Send close msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_DBMCLOSE, fd, 0) ;
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int  clientstub_dbmstore ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
    int  ret = 0 ;
    int  status, buffer_size ;
    long remaining_size, current_size ;

    // Send write msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_DBMSTORE, fd, count_key) ;
    }

    // Receive buffer_size
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &buffer_size, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: buffer_size not received :-(", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
	if (buffer_size < 0) {
	    return -1 ;
	}
    }

    // Send key
    current_size   = 0 ;
    remaining_size = count_key ;
    while ( (ret > 0) && (remaining_size > 0) )
    {
        if (ret >= 0)
        {
            ret = mfs_comm_send_data_to(wb, 0, buff_key + current_size, buffer_size, MPI_CHAR) ;
            if (ret < 0) {
                mfs_print(DBG_ERROR, "Client[%d]: key cannot be sent :-(", mfs_comm_get_rank(wb)) ;
            }
        }

        current_size   = current_size   + buffer_size ;
        remaining_size = remaining_size - buffer_size ;
    }

    // Send value size (in bytes)
    ret = mfs_comm_send_data_to(wb, 0, &count_val, 1, MPI_INT) ;
    if (ret < 0) {
	mfs_print(DBG_ERROR, "Client[%d]: value size cannot be sent :-(", mfs_comm_get_rank(wb)) ;
    }

    // Send value
    current_size   = 0 ;
    remaining_size = count_val ;
    while ( (ret > 0) && (remaining_size > 0) )
    {
        if (ret >= 0)
        {
            ret = mfs_comm_send_data_to(wb, 0, buff_val + current_size, buffer_size, MPI_CHAR) ;
            if (ret < 0) {
                mfs_print(DBG_ERROR, "Client[%d]: value cannot be sent :-(", mfs_comm_get_rank(wb)) ;
            }
        }

        current_size   = current_size   + buffer_size ;
        remaining_size = remaining_size - buffer_size ;
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return bytes written
    return status ;
}

int  clientstub_dbmfetch ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
    int  ret ;

    ret = 0 ;

    // TODO !

    // Return OK/KO
    return ret ;
}

int  clientstub_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key )
{
    int  ret ;

    ret = 0 ;

    // TODO !

    // Return OK/KO
    return ret ;
}

