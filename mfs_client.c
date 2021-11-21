
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"


/*
 *  Main
 */

#define MAX_DATA 1024

int send_request ( MPI_Comm server, int *req_id, int req_action )
{
    int ret ;

    ret = MPI_Send(req_id, 1, MPI_INT, 0, req_action, server);
    return (MPI_SUCCESS == ret) ;
}

int main( int argc, char **argv )
{
    MPI_Comm server;
    char port_name[MPI_MAX_PORT_NAME];
    int buff[MAX_DATA] ;

    // Welcome...
    if (argc < 2) {
        fprintf(stderr, "ERROR: server port name required.\n");
        return -1 ;
    }

    // Initialize...
    MPI_Init(&argc, &argv);
    strcpy(port_name, argv[1]);
    MPI_Comm_connect(port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &server);

    // Requests...
    for (int i = 0; i < 5; i++) {
         buff[0] = i ;
         send_request(server, buff, 2) ;
    }
    send_request(server, buff, 1) ;

    // Finalize...
    MPI_Comm_disconnect(&server);
    MPI_Finalize();

    // End of main
    return 0;
}
