
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

#include "mfs_comm.h"


//
// Send request "header"
//

int mfs_comm_request_send ( MPI_Comm server, int rank, int req_action, int req_arg1, int req_arg2 )
{
    int ret ;
    int buff[3] ;

    // pack msg fields
    buff[0] = req_action ;
    buff[1] = req_arg1 ;
    buff[2] = req_arg2 ;

    // send msg
    ret = MPI_Send(buff, 3, MPI_INT, rank, 0, server) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Send fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int mfs_comm_request_receive ( MPI_Comm server, int *req_action, int *req_arg1, int *req_arg2 )
{
    int ret ;
    int buff[3] ;

    // receive msg
    ret = mfs_comm_recv_data_from(server, MPI_ANY_SOURCE, buff, 3, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[COMM]: MPI_Recv fails :-(") ;
        return -1 ;
    }

    // unpack msg fields
    *req_action = buff[0] ;
    *req_arg1   = buff[1] ;
    *req_arg2   = buff[2] ;

    // Return OK/KO
    return ret ;
}


//
// Send/Receive buffer of data
//

int mfs_comm_recv_data_fromany ( MPI_Comm client, void *buff, int size, int datatype )
{
    int ret ;
    MPI_Status status;

    // Get CMD message
    ret = MPI_Recv(buff, size, datatype, MPI_ANY_SOURCE, 0, client, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Recv fails :-(") ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int mfs_comm_recv_data_from ( MPI_Comm client, int rank, void *buff, int size, int datatype )
{
    int ret ;
    MPI_Status status;

    // Get CMD message
    ret = MPI_Recv(buff, size, datatype, rank, 0, client, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Recv fails :-(") ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int mfs_comm_send_data_to  ( MPI_Comm client, int rank, void *buff, int size, int datatype )
{
    int ret ;

    // Send answer
    ret = MPI_Send(buff, size, datatype, rank, 0, client) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Send fails :-(") ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

