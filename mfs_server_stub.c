
/*
 *  Copyright 2020-2021 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


#include "mfs_server_stub.h"


//
// Server stub
//

int serverstub_init ( server_stub_t *wb, int *argc, char ***argv )
{
    int ret ;

    // MPI_Init
    ret = MPI_Init(argc, argv);
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Init fails :-S");
        return -1 ;
    }

    // wb->size = comm_size()
    ret = MPI_Comm_size(MPI_COMM_WORLD, &(wb->size));
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_size fails :-S");
        return -1 ;
    }

    // wb->rank = comm_rank()
    ret = MPI_Comm_rank(MPI_COMM_WORLD, &(wb->rank));
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_rank fails :-S");
        return -1 ;
    }

    // Open server port...
    ret = MPI_Open_port(MPI_INFO_NULL, wb->port_name);
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Open_port fails :-S");
        return -1 ;
    }

    // Write server port...
    ret = mfs_write_server_port(wb->port_name) ;
    if (ret < 0) {
        return -1 ;
    }

    // wb->the_end/active_threads
    wb->the_end = 0 ; // false

    // Return OK
    return 0 ;
}

int serverstub_finalize ( server_stub_t *wb )
{
    int ret ;

    // Close port
    MPI_Close_port(wb->port_name) ;

    // Finalize
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Finalize fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_get_theend ( server_stub_t *wb )
{
    return (0 == wb->the_end) ;
}

int serverstub_set_theend ( server_stub_t *wb, int value )
{
    wb->the_end = value ;

    // Return OK
    return 0 ;
}

int serverstub_accept ( server_stub_t *ab, server_stub_t *wb )
{
    int ret ;

    // *ab = *wb ;
    memmove(ab, wb, sizeof(server_stub_t)) ;

    // Accept
    ret = MPI_Comm_accept(ab->port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &(ab->client)) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_accept fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_request_recv ( server_stub_t *ab, void *buff_int, int size )
{
    int ret ;
    MPI_Status status;

    ret = MPI_Recv(buff_int, size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, ab->client, &status) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int serverstub_request_send ( server_stub_t *ab, void *buff_int, int size )
{
    int ret ;

    ret = MPI_Send(buff_int, size, MPI_INT, 0, 2, ab->client) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

