
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
#include "mpi.h"


/*
 * Shared state among threads
 */

#define MAX_DATA 1024

typedef struct {
    int  size ;
    int  rank ;
    char port_name[MPI_MAX_PORT_NAME] ;
    int  the_end ;
} whiteboard_t ;


/*
 *  Main
 */

int wb_init ( whiteboard_t *wb, int *argc, char ***argv )
{
    int ret ;

    // wb->the_end = false
    wb->the_end = 0 ;

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
    FILE *fd = fopen("mfs_server.port", "w");
    if (fd == NULL) {
        fprintf(stderr, "fopen fails :-S");
        return -1 ;
    }
    fprintf(fd, "%s\n", wb->port_name);
    fclose(fd);

    // Return OK
    return 0 ;
}

int do_srv ( MPI_Comm *client, whiteboard_t *wb )
{
    int again;
    MPI_Status status;
    int buff[MAX_DATA] ;

    again = 1;
    while (again)
    {
          printf(" * Server[%d] receiving...\n", wb->rank);
          MPI_Recv(buff, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, *client, &status);

          switch (status.MPI_TAG)
          {
              case 0:
	           MPI_Comm_free(client);
	           MPI_Close_port(wb->port_name);
	           MPI_Finalize();
	           return 0;

              case 1:
	           MPI_Comm_disconnect(client);
	           again = 0;
	           break;

              case 2: /* do something */
	           printf("Received: %d\n", buff[0]);
	           break;

              default:
	           /* Unexpected message type */
	           MPI_Abort(MPI_COMM_WORLD, 1);
          }
    }

    return MPI_SUCCESS;
}

int main ( int argc, char **argv )
{
    whiteboard_t wb ;
    MPI_Comm client ;
    int ret ;

    // Welcome...
    fprintf(stdout, "\n"
 		    " MPI_PFS (0.2)\n"
		    " -------------\n"
		    "\n");

    // Initialize...
    ret = wb_init(&wb, &argc, &argv) ;
    if (ret < 0) {
        fprintf(stderr, "ERROR: wb_init fails :-S");
        return -1 ;
    }

    // To serve requests...
    while (! wb.the_end)
    {
        printf(" * Server[%d] accepting...\n", wb.rank);
        MPI_Comm_accept(wb.port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &client);

        do_srv(&client, &wb) ;
   }

   // End of main
   return 0 ;
}

