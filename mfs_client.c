
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

/*
 * Includes
 */

#include "mpi.h"
#include <mfs_lib.h>


/*
 * Client stub
 */

typedef struct
{
    // server port and comm
    char port_name[MPI_MAX_PORT_NAME] ;
    MPI_Comm server ;

    // client identification
    int  size ;
    int  rank ;

} client_stub_t ;


int wb_init_client ( client_stub_t *wb, int *argc, char ***argv )
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

int wb_finalize_client ( client_stub_t *wb )
{
    int ret ;

    // Disconnect...
    ret = MPI_Comm_disconnect(&(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_disconnect fails :-S") ;
        return -1 ;
    }

    // Finalize...
    MPI_Finalize() ;

    // Return OK
    return 0 ;
}


/*
 *  Main
 */

int main( int argc, char **argv )
{
    client_stub_t wb ;
    int ret ;
    int i ;

    // Welcome...
    fprintf(stdout, "\n"
 		    " mfs_client\n"
		    " ----------\n"
		    "\n");

    // Initialize...
    fprintf(stdout, "INFO: Client initializing...\n") ;
    ret = wb_init_client(&wb, &argc, &argv) ;
    if (ret < 0) {
        fprintf(stderr, "ERROR: wb_init_client fails :-S") ;
        return -1 ;
    }

    // Requests...
    for (i = 0; i < 5; i++) {
         send_request(wb.server, 2, i) ;
    }
    send_request(wb.server, 1, i) ;

    // Finalize...
    wb_finalize_client(&wb) ;

    return 0;
}

