
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

#include "mfs_comm_socket.h"


//
// Init, Finalize
//

int mfs_comm_socket_init ( comm_t *cb, params_t *params )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_finalize ( comm_t *cb )
{
    // TODO

    // Return OK
    return 1 ;
}


//
// Register, unregister, connect, disconnect
//

int mfs_comm_socket_register ( comm_t *cb )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_unregister ( comm_t *cb )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_accept ( comm_t *ab )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_connect ( comm_t *cb )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_disconnect ( comm_t *cb )
{
    // TODO

    // Return OK
    return 1 ;
}


//
// Send/Receive buffer of data
//

int mfs_comm_socket_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    // TODO

    // Return OK
    return 1 ;
}

int mfs_comm_socket_send_data_to  ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype )
{
    // TODO

    // Return OK
    return 1 ;
}

