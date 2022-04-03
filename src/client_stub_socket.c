
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


#include "client_stub_socket.h"


/*
 *  Auxiliar API
 */

// Action over resource

long clientstub_socket_action_over_named_resource ( comm_t *wb, const char *pathname, int pathname_size, int opt, int action )
{
    int   ret = 0 ;
    long  status ;

    // Send action msg
    if (ret >= 0)
    {
         ret = mfs_comm_socket_send_request(wb, 0, action, pathname_size, opt, 0) ;
    }

    // Send pathname
    if (ret >= 0)
    {
        ret = mfs_comm_socket_send_data_to(wb, 0, (void *)pathname, pathname_size) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(long)) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return status
    return status ;
}

int clientstub_socket_action_over_fd_resource ( comm_t *wb, long fd, int opt, int action )
{
    int   ret = 0 ;
    int   status ;

    // Send action msg
    if (ret >= 0)
    {
         ret = mfs_comm_socket_send_request(wb, 0, action, fd, opt, 0) ;
    }

    // Receive status
    if (ret >= 0)
    {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

// Send buffer

int clientstub_socket_action_send_buffer ( comm_t *wb, void *buff_char, int count, int buffer_size )
{
    int  ret ;
    long remaining_size, current_size ;

    ret            =  1 ;
    current_size   =  0 ;
    remaining_size = count ;
    while ( (ret > 0) && (remaining_size > 0) )
    {
        // Send data
        if (ret >= 0)
        {
            ret = mfs_comm_socket_send_data_to(wb, 0, buff_char + current_size, buffer_size) ;
            if (ret < 0) {
                mfs_print(DBG_ERROR, "Client[%d]: data cannot be sent :-(\n", mfs_comm_get_rank(wb)) ;
            }
        }

        current_size   = current_size   + buffer_size ;
        remaining_size = remaining_size - buffer_size ;
    }

    // Return bytes written
    return current_size ;
}


/*
 *  File System API
 */

int clientstub_socket_init ( comm_t *wb, params_t *params )
{
	int ret ;

        // Check params...
        NULL_PRT_MSG_RET_VAL(wb, "Client[-1]: NULL wb :-(\n", -1) ;

	// Initialize fields
        mfs_comm_reset(wb) ;
        wb->comm_protocol      = COMM_USE_SOCKET ;
        wb->comm_protocol_name = "SOCKET" ;
	wb->ns_backend         = params->ns_backend ;
	wb->sd                 = -1 ;

        // Return OK
        return 1 ;
}

int clientstub_socket_finalize ( comm_t *wb, params_t *params )
{
    int   ret = 0 ;

    // Close socket
    if (ret >= 0)
    {
        ret = base_socket_close(&(wb->sd)) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: finalization fails :-(\n", mfs_comm_get_rank(wb)) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int clientstub_socket_open_partition_element ( comm_t *wb, params_t *params, conf_part_t *partition, int remote_rank )
{
    int    ret = 0 ;
    char  *srv_uri ;
    int    sd, port_name_size ;
    struct hostent *hp ;
    int    srv_port ;

    // Initialize
    wb->dd = -1 ;
    strcpy(wb->port_name, "") ;

    // Get srv_uri and translate srv_uri into (host + port)
    srv_uri = info_fsconf_get_partition_node(partition, remote_rank) ;

    port_name_size = MPI_MAX_PORT_NAME ;
    ret = info_ns_lookup(wb, wb->ns_backend, srv_uri, wb->port_name, &port_name_size) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[-1]: info_ns_lookup fails for %d element of the partition with uri:'%s' and using backend:%d :-(\n", remote_rank, srv_uri, wb->ns_backend) ;
        return -1 ;
    }

    ret = info_ns_split_portname(wb->port_name, &hp, &srv_port) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[-1]: info_ns_split_portname(%s, ...) fails :-(\n", wb->port_name) ;
        return -1 ;
    }

    // socket
    wb->dd = base_socket_connect(hp, srv_port) ;
    if (wb->dd < 0) {
        mfs_print(DBG_ERROR, "Client[-1]: base_socket_connect fails :-(\n") ;
        return -1 ;
    }

    wb->is_connected = 1 ;

    // Return OK/KO
    return ret ;
}

int clientstub_socket_close_partition_element ( comm_t *wb, params_t *params, conf_part_t *partition )
{
    int   ret = 0 ;

    // Remote disconnect...
    if (ret >= 0)
    {
         ret = mfs_comm_socket_send_request(wb, 0, REQ_ACTION_DISCONNECT, 0, 0, 0) ;
    }

    // Disconnect...
    if (ret >= 0)
    {
	ret = base_socket_close(&(wb->dd)) ;
	if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: disconnect %d fails :-(\n", mfs_comm_get_rank(wb), wb->dd) ;
	}
    }

    wb->is_connected = 0 ;

    // Return OK/KO
    return ret ;
}


/*
 *  File API
 */

long clientstub_socket_open ( comm_t *wb, const char *pathname, int flags )
{
     return clientstub_socket_action_over_named_resource(wb, pathname, strlen(pathname) + 1, flags, REQ_ACTION_OPEN) ;
}

int  clientstub_socket_close ( comm_t *wb, long fd )
{
     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb, "[CLNT_STUB] NULL wb :-/\n", -1) ;
      NEG_PRT_MSG_RET_VAL(fd, "[CLNT_STUB] fd < 0  :-/\n", -1) ;

     return clientstub_socket_action_over_fd_resource(wb, fd, 0, REQ_ACTION_CLOSE) ;
}

int  clientstub_socket_read ( comm_t *wb, long fd, void *buff_char, int count )
{
     int  ret = 0 ;
     int  status ;
     long remaining_size, current_size ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,        "[CLNT_STUB] NULL wb        :-/\n", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,        "[CLNT_STUB] fd < 0         :-/\n", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_char, "[CLNT_STUB] NULL buff_char :-/\n", -1) ;

     // Send read msg
     if (ret >= 0)
     {
         ret = mfs_comm_socket_send_request(wb, 0, REQ_ACTION_READ, fd, count, 0) ;
     }

     current_size   = 0 ;
     remaining_size = count ;
     while ( (ret >= 0) && (remaining_size > 0) )
     {
          // Receive status
          if (ret >= 0)
          {
              ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
              if (ret < 0) {
                  mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
              }
          }

          // Receive data
          if ( (ret >= 0) && (status > 0) )
          {
              ret = mfs_comm_socket_recv_data_from(wb, 0, buff_char + current_size, status) ;
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

int  clientstub_socket_write ( comm_t *wb, long fd, void *buff_char, int count )
{
     int  ret, status ;
     int  buffer_size ;
     long remaining_size, current_size ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,        "[CLNT_STUB] NULL wb        :-/\n", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,        "[CLNT_STUB] fd < 0         :-/\n", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_char, "[CLNT_STUB] NULL buff_char :-/\n", -1) ;

     ret    = 0 ;
     status = -1 ;

     // Send write msg
     if (ret >= 0)
     {
         ret = mfs_comm_socket_send_request(wb, 0, REQ_ACTION_WRITE, fd, count, 0) ;
     }

     // Receive buffer_size
     if (ret >= 0)
     {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &buffer_size, sizeof(int)) ;
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
        ret = clientstub_socket_action_send_buffer(wb, buff_char, count, buffer_size) ;
     }

     // Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
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

long clientstub_socket_mkdir ( comm_t *wb, const char *pathname, int mode )
{
     return clientstub_socket_action_over_named_resource(wb, pathname, strlen(pathname) + 1, mode, REQ_ACTION_MKDIR) ;
}

long clientstub_socket_rmdir ( comm_t *wb, const char *pathname )
{
     return clientstub_socket_action_over_named_resource(wb, pathname, strlen(pathname) + 1,    0, REQ_ACTION_RMDIR) ;
}


/*
 *  DBM File API
 */

long clientstub_socket_dbmopen ( comm_t *wb, const char *pathname, int flags )
{
     return clientstub_socket_action_over_named_resource(wb, pathname, strlen(pathname) + 1, flags, REQ_ACTION_DBMOPEN) ;
}

int  clientstub_socket_dbmclose ( comm_t *wb, long fd )
{
     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb, "[CLNT_STUB] NULL wb :-/\n", -1) ;
      NEG_PRT_MSG_RET_VAL(fd, "[CLNT_STUB] fd < 0  :-/\n", -1) ;

     return clientstub_socket_action_over_fd_resource(wb, fd, 0, REQ_ACTION_DBMCLOSE) ;
}

int  clientstub_socket_dbmstore ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
     int  ret, status  ;
     long remaining_size, current_size ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,       "[CLNT_STUB] NULL wb       :-/\n", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,       "[CLNT_STUB] fd < 0        :-/\n", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_key, "[CLNT_STUB] NULL buff_key :-/\n", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_val, "[CLNT_STUB] NULL buff_val :-/\n", -1) ;

     // Set initial values...
     ret = 0 ;
     status = -1 ;

     // Send write msg
     if (ret >= 0)
     {
    	 mfs_print(DBG_INFO, "Client[%d]: dbmstore fd:%ld key_size:%d >> server\n", mfs_comm_get_rank(wb), fd, count_key) ;
         ret = mfs_comm_socket_send_request(wb, 0, REQ_ACTION_DBMSTORE, fd, count_key, count_val) ;
     }

     // Receive status
     if (ret >= 0)
     {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
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
        clientstub_socket_action_send_buffer(wb, buff_key, count_key, count_key) ;

    	mfs_print(DBG_INFO, "Client[%d]: dbmstore buff_val:%d >> server\n", mfs_comm_get_rank(wb), count_val) ;
        clientstub_socket_action_send_buffer(wb, buff_val, count_val, count_val) ;
     }

     // Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

    	mfs_print(DBG_INFO, "Client[%d]: dbmstore status:%d << server\n", mfs_comm_get_rank(wb), status) ;
     }

     // Return bytes written
     return status ;
}

int  clientstub_socket_dbmfetch ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int  ret, status  ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,       "[CLNT_STUB] NULL wb       :-/\n", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,       "[CLNT_STUB] fd < 0        :-/\n", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_key, "[CLNT_STUB] NULL buff_key :-/\n", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_val, "[CLNT_STUB] NULL buff_val :-/\n", -1) ;

     // Set initial values...
     ret = 0 ;
     status = -1 ;

     // (1) Send write msg
     if (ret >= 0)
     {
         ret = mfs_comm_socket_send_request(wb, 0, REQ_ACTION_DBMFETCH, fd, count_key, *count_val) ;
     }

     // (2) Receive status
     if (ret >= 0)
     {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
	if (status < 0) {
	    return -1 ;
	}
     }

     // (3) Send key
     if (ret >= 0)
     {
        clientstub_socket_action_send_buffer(wb, buff_key, count_key, count_key) ;
     }

     // (4) Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }

        // if remote error then return
        if (status < 0) {
            return -1 ;
        }
     }

     // (5) Receive val
     *count_val = status ;
     ret = mfs_comm_socket_recv_data_from(wb, 0, buff_val, *count_val) ;
     if (ret < 0) {
         mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
     }

     // Return bytes written
     return status ;
}

int  clientstub_socket_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key )
{
     int  ret, status  ;

     // Check arguments...
     NULL_PRT_MSG_RET_VAL(wb,       "[CLNT_STUB] NULL wb       :-/\n", -1) ;
      NEG_PRT_MSG_RET_VAL(fd,       "[CLNT_STUB] fd < 0        :-/\n", -1) ;
     NULL_PRT_MSG_RET_VAL(buff_key, "[CLNT_STUB] NULL buff_key :-/\n", -1) ;

     // Set initial values...
     ret = 0 ;
     status = -1 ;

     // Send write msg
     if (ret >= 0)
     {
         ret = mfs_comm_socket_send_request(wb, 0, REQ_ACTION_DBMDELETE, fd, count_key, 0) ;
     }

     // Send key
     if (ret >= 0)
     {
        clientstub_socket_action_send_buffer(wb, buff_key, count_key, count_key) ;
     }

     // Receive status
     //if (ret >= 0)
     {
        ret = mfs_comm_socket_recv_data_from(wb, 0, &status, sizeof(int)) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: operation status not received :-(\n", mfs_comm_get_rank(wb)) ;
        }
     }

     // Return bytes written
     return status ;
}

