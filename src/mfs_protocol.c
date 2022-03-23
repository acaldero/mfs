
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

#include "mfs_protocol.h"


//
// Send request "header"
//

int mfs_protocol_request_send ( comm_t *cb, int rank, msg_t *msg )
{
    int ret ;

    // Send msg
    ret = mfs_comm_send_data_to(cb, rank, msg, 3, MPI_LONG) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[PROTOCOL]: mfs_comm_send_data_to fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int mfs_protocol_request_receive ( comm_t *cb, msg_t *msg )
{
    int ret ;

    // Receive msg
    ret = mfs_comm_recv_data_from(cb, MPI_ANY_SOURCE, msg, 3, MPI_LONG) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[PROTOCOL]: mfs_comm_recv_data_from fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

