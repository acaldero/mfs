
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


#include <mfs_client_stub.h>


int clientstub_init ( client_stub_t *wb, int *argc, char ***argv )
{
    int ret ;

    // Read server port...
    ret = mfs_read_server_port(wb->port_name) ;
    if (ret < 0) {
        fprintf(stderr, "ERROR: server port name required in the 'mfs_server.port' file.\n");
        return -1 ;
    }

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

    // Connect...
    ret = MPI_Comm_connect(wb->port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_connect fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int clientstub_finalize ( client_stub_t *wb )
{
    int ret ;

    // Disconnect...
    ret = MPI_Comm_disconnect(&(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_disconnect fails :-S") ;
        return -1 ;
    }

    // Finalize...
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Finalize fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int clientstub_request ( client_stub_t *wb, int req_action, int req_id )
{
    int ret ;
    int buff[2] ;

    buff[0] = req_action ;
    buff[1] = req_id ;
    ret = MPI_Send(buff, 2, MPI_INT, 0, 2, wb->server) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

