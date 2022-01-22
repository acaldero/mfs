
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

#include "mfs_protocol.h"


//
// Send request "header"
//

int mfs_protocol_request_send ( comm_t *cb, int rank, msg_t *msg )
{
    int ret ;

    // Send msg
    ret = mfs_comm_send_data_to(cb, rank, msg, 3, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[PROTOCOL]: MPI_Send fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int mfs_protocol_request_receive ( comm_t *cb, msg_t *msg )
{
    int ret ;

    // Receive msg
    ret = mfs_comm_recv_data_from(cb, MPI_ANY_SOURCE, msg, 3, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[PROTOCOL]: MPI_Recv fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int mfs_protocol_request_do ( comm_t *wb, buffer_t *info, int neltos )
{
    int ret ;

    ret = 1 ;
    for (int i=0; i<neltos; i++)
    {
	 if (COM_SEND_DATA_TO == info.comm_action)
             ret =   mfs_comm_send_data_to(wb, 0, info[i].buff, info[i].size, info[i].datatype) ;
	 if (COM_RECV_DATA_FROM == info.comm_action)
	     ret = mfs_comm_recv_data_from(wb, 0, info[i].buff, info[i].size, info[i].datatype) ;

	 if (ret < 0) {
	     mfs_print(DBG_ERROR, info[i].err_msg, wb->rank) ;
	     return -1 ;
	 }
    }

    // Return OK/KO
    return ret ;
}
