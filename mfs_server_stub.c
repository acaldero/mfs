
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


int serverstub_init ( server_stub_t *wb, int *argc, char ***argv )
{
    int ret, claimed, provided ;

    // MPI_Init
    ret = MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Init fails :-S");
        return -1 ;
    }

    MPI_Query_thread(&claimed) ;
    if (claimed != provided) {
        fprintf(stderr, "WARNING: MPI_Init_thread with only %s :-S", provided);
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
    ret = mfs_write_server_port(wb->port_name, wb->rank) ;
    if (ret < 0) {
        return -1 ;
    }

    // wb->the_end, tag_id
    wb->tag_id = 1 ; // initial tag_id, autoincrement latter

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

int serverstub_accept ( server_stub_t *ab, server_stub_t *wb )
{
    int ret ;

    // *ab = *wb ;
    wb->tag_id = wb->tag_id + 1 ;
    memmove(ab, wb, sizeof(server_stub_t)) ;

    // Accept
    ret = MPI_Comm_accept(ab->port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &(ab->client)) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_accept fails :-S") ;
        return -1 ;
    }

    // send tag_id
    ret = MPI_Send(&(ab->tag_id), 1, MPI_INT, 0, 0, ab->client) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Send fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_request_recv ( server_stub_t *ab, void *buff, int size, int datatype )
{
    int ret ;
    MPI_Status status;

    // Get CMD message
    ret = MPI_Recv(buff, size, datatype, MPI_ANY_SOURCE, ab->tag_id, ab->client, &status) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int serverstub_request_send ( server_stub_t *ab, void *buff, int size, int datatype )
{
    int ret ;

    // Send answer
    ret = MPI_Send(buff, size, datatype, 0, ab->tag_id, ab->client) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

