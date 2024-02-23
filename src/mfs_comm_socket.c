
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#include "mfs_comm_socket.h"


//
// Send/Receive buffer of data
//

int mfs_comm_socket::recv_data_from ( int rank, void *buff, int size )
{
        int ret = -1 ;

        // Check arguments
        if (-1 == this->dd) { return -1; }

        // Try to receive data from...
//TODO  ret = mfs_file_posix_read(this->dd, buff, size) ;
	if (ret < 0) {
	    mfs_print(DBG_ERROR, "[COMM]: read from socket %d fails :-(\n", this->dd) ;
	}

	// Stats
	this->n_recv_req++ ;

        // Return OK/KO
        return ret ;
}

int mfs_comm_socket::send_data_to  ( int rank, void *buff, int size )
{
        int ret = -1;

        // Check arguments
        if (-1 == this->dd) { return -1; }

        // Send data to...
//TODO  ret = mfs_file_posix_write(this->dd, buff, size) ;

        // Stats
        this->n_send_req++ ;

        // Return OK/KO
        return ret ;
}


//
// Requests
//

int mfs_comm_socket::send_request ( int rank, long action, long arg1, long arg2, long arg3 )
{
    long msg[5] ;

    msg[0] = action ;
    msg[1] = arg1 ;
    msg[2] = arg2 ;
    msg[3] = arg3 ;

    return send_data_to(rank, (char *)(msg), sizeof(msg)) ;
}

int mfs_comm_socket::receive_request ( int rank, long *action, long *arg1, long *arg2, long *arg3 )
{
    int ret ;
    long msg[5] ;

    ret = recv_data_from(rank, msg, sizeof(msg)) ;

    (*action) = msg[0] ;
    (*arg1)   = msg[1] ;
    (*arg2)   = msg[2] ;
    (*arg3)   = msg[3] ;

    return  ret ;
}


