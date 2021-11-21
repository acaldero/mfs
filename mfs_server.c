
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

struct {
   int the_end ;
} whiteboard ;


/*
 *  Main
 */

int main ( int argc, char **argv )
{
   MPI_Comm client;
   MPI_Status status;
   char port_name[MPI_MAX_PORT_NAME];
   int size, again, i;
   int ret = MPI_SUCCESS;

   // Welcome...
   fprintf(stdout, "\n"
		   " MPI_PFS (0.1)\n"
		   " -------------\n"
		   "\n");

   // Initialize...
   ret = MPI_Init(&argc, &argv);
   if (MPI_SUCCESS != ret) {
       fprintf(stderr, "MPI_Open_port fails :-S");
       return -1 ;
   }

   MPI_Comm_size(MPI_COMM_WORLD, &size);
   if (size != 1) {
       fprintf(stderr, "Server too big");
       return -1 ;
   }

   // Open server port...
   ret = MPI_Open_port(MPI_INFO_NULL, port_name);
   if (MPI_SUCCESS != ret) {
       fprintf(stderr, "MPI_Open_port fails :-S");
       return -1 ;
   }

   printf(" * Server port: %s\n", port_name);

   // To serve requests...
   whiteboard.the_end = 0 ;
   while (! whiteboard.the_end)
   {
       MPI_Comm_accept(port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &client);
       again = 1;
       while (again)
       {
         MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, client, &status);
         switch (status.MPI_TAG)
         {
              case 0:
	        MPI_Comm_free(&client);
	        MPI_Close_port(port_name);
	        MPI_Finalize();
	        return 0;

              case 1:
	        MPI_Comm_disconnect(&client);
	        again = 0;
	        break;

              case 2: /* do something */
	        printf("Received: %d\n", i);
	        break;

              default:
	        /* Unexpected message type */
	        MPI_Abort(MPI_COMM_WORLD, 1);
         }
    }
  } // while (! whiteboard.the_end)

  // End of main
  return 0 ;
}

