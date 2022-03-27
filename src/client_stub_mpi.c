
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MFS.
 *
 *  MFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "client_stub_mpi.h"


/*
 *  Auxiliar API
 */

// Action over resource


long clientstub_mpi_action_over_named_resource ( comm_t *wb, const char *pathname, int pathname_size, int opt, int action )
{
    int   ret = 0 ;
    long  status ;

    // Send action msg
    if (ret >= 0)
    {
        ret = mfs_comm_mpi_send_request(wb, 0, action, pathname_size, opt, 0) ;
    }

    // Send pathname
    if (ret >= 0)
    {
        ret = mfs_comm_mpi_send_data_to(wb, 0, (void *)pathname, pathname_size, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_LONG) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return status
    return status ;
}

int clientstub_mpi_action_over_fd_resource ( comm_t *wb, long fd, int opt, int action )
{
    int   ret = 0 ;
    int   status ;

    // Send action msg
    if (ret >= 0)
    {
        ret = mfs_comm_mpi_send_request(wb, 0, action, fd, opt, 0) ;
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File System API
 */

int clientstub_mpi_init ( comm_t *wb, params_t *params, conf_t *conf )
{
    int    ret = 0 ;
    int    remote_rank ;
    char  *srv_uri ;

    // Initialize
    if (ret >= 0)
    {
        ret = mfs_comm_mpi_init(wb, conf->active, params->argc, params->argv) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: initialization fails :-(\n", -1) ;
        }
    }

    // Connect to service
    if (ret >= 0)
    {
        // Get service name
        remote_rank = mfs_comm_get_rank(wb) % conf->active->n_nodes ;
        srv_uri     = info_fsconf_get_active_node(conf, remote_rank) ;
        strcpy(wb->srv_name, srv_uri) ;

        // Lookup...
        ret = MPI_Lookup_name(srv_uri, MPI_INFO_NULL, wb->port_name) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "Client[%d]: MPI_Lookup_name for '%s' fails :-(\n", -1, srv_uri) ;
        }

        // Connect...
        ret = MPI_Comm_connect(wb->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(wb->endpoint)) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "Client[%d]: MPI_Comm_connect with '%s' fails :-(\n", -1, srv_uri) ;
        }

        wb->is_connected = 1 ;
    }

    // Return OK/KO
    return ret ;
}

int clientstub_mpi_finalize ( comm_t *wb, params_t *params )
{
    int   ret = 0 ;

    // Remote disconnect...
    if (ret >= 0)
    {
        ret = mfs_comm_mpi_send_request(wb, 0, REQ_ACTION_DISCONNECT, 0, 0, 0) ;
    }

    // Disconnect...
    if (ret >= 0)
    {
        ret = MPI_Comm_disconnect(&(wb->endpoint)) ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "Client[%d]: disconnect fails :-(\n", mfs_comm_get_rank(wb)) ;
        }

        wb->is_connected = 0 ;
    }

    // Finalize comms...
    if (ret >= 0)
    {
        // Finalize
        ret = MPI_Finalize() ;
        if (MPI_SUCCESS != ret) {
            mfs_print(DBG_ERROR, "Client[%d]: finalization fails :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Finalize params
    if (ret >= 0)
    {
         info_params_free(params) ;
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File API
 */

long clientstub_mpi_open ( comm_t *wb, const char *pathname, int flags )
{
     return clientstub_mpi_action_over_named_resource(wb, pathname, strlen(pathname) + 1, flags, REQ_ACTION_OPEN) ;
}

int  clientstub_mpi_close ( comm_t *wb, long fd )
{
     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb, "[CLNT_STUB] NULL wb :-/", -1) ;
      NEG_PRT_MSG_RET_VAL(fd, "[CLNT_STUB] fd < 0  :-/", -1) ;

     return clientstub_mpi_action_over_fd_resource(wb, fd, 0, REQ_ACTION_CLOSE) ;
}

int  clientstub_mpi_read ( comm_t *wb, long fd, void *buff_char, int count )
{
     int  ret = 0 ;
     int  status ;
     long remaining_size, current_size ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,        "[CLNT_STUB] NULL wb        :-/", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,        "[CLNT_STUB] fd < 0  :-/", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_char, "[CLNT_STUB] NULL buff_char :-/", -1) ;

     // Send read msg
     if (ret >= 0)
     {
         ret = mfs_comm_mpi_send_request(wb, 0, REQ_ACTION_READ, fd, count, 0) ;
     }

     current_size   = 0 ;
     remaining_size = count ;
     while ( (ret >= 0) && (remaining_size > 0) )
     {
          // Receive status
          if (ret >= 0)
          {
              ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
              if (ret < 0) {
                  mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
              }
          }

          // Receive data
          if ( (ret >= 0) && (status > 0) )
          {
              ret = mfs_comm_mpi_recv_data_from(wb, 0, buff_char + current_size, status, MPI_CHAR) ;
              if (ret < 0) {
    	          mfs_print(DBG_ERROR, "Client[%d]: data not received :-(\n", mfs_comm_get_rank(wb)) ;
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

int  clientstub_mpi_write ( comm_t *wb, long fd, void *buff_char, int count )
{
     int  ret, status ;
     int  buffer_size ;
     long remaining_size, current_size ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,        "[CLNT_STUB] NULL wb        :-/", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,        "[CLNT_STUB] fd < 0         :-/", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_char, "[CLNT_STUB] NULL buff_char :-/", -1) ;

     ret    = 0 ;
     status = -1 ;

     // Send write msg
     if (ret >= 0)
     {
         ret = mfs_comm_mpi_send_request(wb, 0, REQ_ACTION_WRITE, fd, count, 0) ;
     }

     // Receive buffer_size
     if (ret >= 0)
     {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &buffer_size, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: buffer_size not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
	if (buffer_size < 0) {
	    return -1 ;
	}
     }

     // Send buffer and receive status
     if (ret >= 0)
     {
        ret = mfs_comm_mpi_send_buffer_in_chunks(wb, buff_char, count, buffer_size) ;
     }

     // Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }
     }

     // Return bytes written
     return status ;
}


/*
 *  Directory API
 */

long clientstub_mpi_mkdir ( comm_t *wb, const char *pathname, int mode )
{
     return clientstub_mpi_action_over_named_resource(wb, pathname, strlen(pathname) + 1, mode, REQ_ACTION_MKDIR) ;
}

long clientstub_mpi_rmdir ( comm_t *wb, const char *pathname )
{
     return clientstub_mpi_action_over_named_resource(wb, pathname, strlen(pathname) + 1,    0, REQ_ACTION_RMDIR) ;
}


/*
 *  DBM File API
 */

long clientstub_mpi_dbmopen ( comm_t *wb, const char *pathname, int flags )
{
     return clientstub_mpi_action_over_named_resource(wb, pathname, strlen(pathname) + 1, flags, REQ_ACTION_DBMOPEN) ;
}

int  clientstub_mpi_dbmclose ( comm_t *wb, long fd )
{
     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb, "[CLNT_STUB] NULL wb :-/", -1) ;
      NEG_PRT_MSG_RET_VAL(fd, "[CLNT_STUB] fd < 0  :-/", -1) ;

     return clientstub_mpi_action_over_fd_resource(wb, fd, 0, REQ_ACTION_DBMCLOSE) ;
}

int  clientstub_mpi_dbmstore ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
     int  ret, status  ;
     long remaining_size, current_size ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,       "[CLNT_STUB] NULL wb       :-/", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,       "[CLNT_STUB] fd < 0        :-/", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_key, "[CLNT_STUB] NULL buff_key :-/", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_val, "[CLNT_STUB] NULL buff_val :-/", -1) ;

     // Set initial values...
     ret = 0 ;
     status = -1 ;

     // Send write msg
     if (ret >= 0)
     {
    	 mfs_print(DBG_INFO, "Client[%d]: dbmstore fd:%ld key_size:%d >> server\n", mfs_comm_get_rank(wb), fd, count_key) ;
         ret = mfs_comm_mpi_send_request(wb, 0, REQ_ACTION_DBMSTORE, fd, count_key, 0) ;
     }

     // Send value size (in bytes)
     if (ret >= 0)
     {
    	mfs_print(DBG_INFO, "Client[%d]: dbmstore val_size:%d >> server\n", mfs_comm_get_rank(wb), count_val) ;
        ret = mfs_comm_mpi_send_data_to(wb, 0, &count_val, 1, MPI_INT) ;
        if (ret < 0) {
    	    mfs_print(DBG_ERROR, "Client[%d]: value size cannot be sent :-(\n", mfs_comm_get_rank(wb)) ;
        }
     }

     // Receive status
     if (ret >= 0)
     {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
	if (status < 0) {
	    return -1 ;
	}
     }

     // Send key + value
     if (ret >= 0)
     {
    	mfs_print(DBG_INFO, "Client[%d]: dbmstore buff_key:%d >> server\n", mfs_comm_get_rank(wb), count_key) ;
        mfs_comm_mpi_send_buffer_in_chunks(wb, buff_key, count_key, count_key) ;

    	mfs_print(DBG_INFO, "Client[%d]: dbmstore buff_val:%d >> server\n", mfs_comm_get_rank(wb), count_val) ;
        mfs_comm_mpi_send_buffer_in_chunks(wb, buff_val, count_val, count_val) ;
     }

     // Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

    	mfs_print(DBG_INFO, "Client[%d]: dbmstore status:%d << server\n", mfs_comm_get_rank(wb), status) ;
     }

     // Return bytes written
     return status ;
}

int  clientstub_mpi_dbmfetch ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int  ret, status  ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,       "[CLNT_STUB] NULL wb       :-/", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,       "[CLNT_STUB] fd < 0        :-/", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_key, "[CLNT_STUB] NULL buff_key :-/", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_val, "[CLNT_STUB] NULL buff_val :-/", -1) ;

     // Set initial values...
     ret = 0 ;
     status = -1 ;

     // (1) Send write msg
     if (ret >= 0)
     {
         ret = mfs_comm_mpi_send_request(wb, 0, REQ_ACTION_DBMFETCH, fd, count_key, 0) ;
     }

     // (2) Send value size (in bytes)
     if (ret >= 0)
     {
        ret = mfs_comm_mpi_send_data_to(wb, 0, count_val, 1, MPI_INT) ;
        if (ret < 0) {
    	    mfs_print(DBG_ERROR, "Client[%d]: value size cannot be sent :-(\n", mfs_comm_get_rank(wb)) ;
        }
     }

     // (3) Receive status
     if (ret >= 0)
     {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
	if (status < 0) {
	    return -1 ;
	}
     }

     // (4) Send key
     if (ret >= 0)
     {
        mfs_comm_mpi_send_buffer_in_chunks(wb, buff_key, count_key, count_key) ;
     }

     // (5) Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
	if (status < 0) {
	    return -1 ;
	}
     }

     // (6) Receive val
     *count_val = status ;
     ret = mfs_comm_mpi_recv_data_from(wb, 0, buff_val, *count_val, MPI_CHAR) ;
     if (ret < 0) {
         mfs_print(DBG_ERROR, "Client[%d]: value of %d bytes not received :-(\n", mfs_comm_get_rank(wb), *count_val) ;
     }

     // Return bytes written
     return status ;
}

int  clientstub_mpi_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key )
{
     int  ret, status  ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,       "[CLNT_STUB] NULL wb       :-/", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,       "[CLNT_STUB] fd < 0        :-/", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_key, "[CLNT_STUB] NULL buff_key :-/", -1) ;

     // Set initial values...
     ret = 0 ;
     status = -1 ;

     // Send write msg
     if (ret >= 0)
     {
         ret = mfs_comm_mpi_send_request(wb, 0, REQ_ACTION_DBMDELETE, fd, count_key, 0) ;
     }

     // Send key
     if (ret >= 0)
     {
        mfs_comm_mpi_send_buffer_in_chunks(wb, buff_key, count_key, count_key) ;
     }

     // Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_mpi_recv_data_from(wb, 0, &status, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }
     }

     // Return bytes written
     return status ;
}

