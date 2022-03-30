
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

#include "mfs_comm_mpi.h"


//
// Send/Receive buffer of data
//

int mfs_comm_mpi_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;
    MPI_Status status;
    int msg_size ;

    // Check if there is a CMD message
    ret = MPI_Probe(rank, 0, cb->endpoint, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Probe fails :-(\n") ;
        return -1 ;
    }

    MPI_Get_count(&status, datatype, &msg_size) ;
    if (msg_size != size) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Probe detects an unexpected message of %d (but not %d):-(\n", msg_size, size) ;
        return -1 ;
    }

    // Get CMD message
    ret = MPI_Recv(buff, size, datatype, rank, 0, cb->endpoint, &status) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Recv fails :-(\n") ;
        return -1 ;
    }

    // Copy status
    cb->status_rank = status.MPI_SOURCE ;
    cb->status_tag  = status.MPI_TAG ;
    MPI_Get_count(&status, datatype, &(cb->status_count));

    // Stats
    cb->n_recv_req++ ;

    // Return OK
    return 1 ;
}

int mfs_comm_mpi_send_data_to  ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    int ret ;

    // Send answer
    ret = MPI_Send(buff, size, datatype, rank, 0, cb->endpoint) ;
    if (MPI_SUCCESS != ret) {
        mfs_print(DBG_WARNING, "[COMM]: MPI_Send fails :-(\n") ;
        return -1 ;
    }

    // Stats
    cb->n_send_req++ ;

    // Return OK
    return 1 ;
}


//
// Send buffer in chunks of buffer_size bytes
//

int mfs_comm_mpi_send_buffer_in_chunks ( comm_t *wb, void *buff_char, int count, int buffer_size )
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
            ret = mfs_comm_mpi_send_data_to(wb, 0, buff_char + current_size, buffer_size, MPI_CHAR) ;
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


//
// Requests
//

int mfs_comm_mpi_send_request ( comm_t *wb, int rank, long action, long arg1, long arg2, long arg3 )
{
    msg_t msg ;

    msg.req_action = action ;
    msg.req_arg1 = arg1 ;
    msg.req_arg2 = arg2 ;
    msg.req_arg3 = arg3 ;

    return mfs_comm_mpi_send_data_to(wb, rank, (char *)&(msg), 4, MPI_LONG) ;
}

int mfs_comm_mpi_receive_request ( comm_t *wb, int rank, msg_t *msg )
{
    return mfs_comm_mpi_recv_data_from(wb, rank, msg, 4, MPI_LONG) ;
}

