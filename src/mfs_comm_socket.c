
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

#include "mfs_comm_socket.h"


//
// Send/Receive buffer of data
//

int mfs_comm_socket_recv_data_from ( comm_t *cb, int rank, void *buff, int size )
{
        int ret ;

        // Check arguments
	NULL_PRT_MSG_RET_VAL(cb, "[COMM]: NULL cb     :-(\n", -1) ;
        if (-1 == cb->dd) { return -1; }

        // Try to receive data from...
        ret = mfs_file_posix_read(cb->dd, buff, size) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: read from socket %d fails :-(\n", cb->dd) ;
	}

	// Stats
	cb->n_recv_req++ ;

        // Return OK/KO
        return ret ;
}

int mfs_comm_socket_send_data_to  ( comm_t *cb, int rank, void *buff, int size )
{
        int ret ;

        // Check arguments
	NULL_PRT_MSG_RET_VAL(cb, "[COMM]: NULL cb     :-(\n", -1) ;
        if (-1 == cb->dd) { return -1; }

        // Send data to...
        ret = mfs_file_posix_write(cb->dd, buff, size) ;

        // Stats
        cb->n_send_req++ ;

        // Return OK/KO
        return ret ;
}


//
// Requests
//

int mfs_comm_socket_send_request ( comm_t *wb, int rank, long action, long arg1, long arg2, long arg3 )
{
    msg_t msg ;

    msg.req_action = action ;
    msg.req_arg1 = arg1 ;
    msg.req_arg2 = arg2 ;
    msg.req_arg3 = arg3 ;

    return mfs_comm_socket_send_data_to(wb, rank, (char *)&(msg), sizeof(msg_t)) ;
}

int mfs_comm_socket_receive_request ( comm_t *wb, int rank, msg_t *msg )
{
    return mfs_comm_socket_recv_data_from(wb, rank, msg, sizeof(msg_t)) ;
}

