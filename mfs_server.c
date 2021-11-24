
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


#include <mfs_server_stub.h>


int do_srv ( MPI_Comm *client, server_stub_t *wb )
{
    int again;
    int req_action ;
    int req_id ;

    again = 1;
    while (again)
    {
          printf("INFO: Server[%d] receiving...\n", wb->rank);
          serverstub_request_recv(*client, &req_action, &req_id) ;
          switch (req_action)
          {
              case REQ_ACTION_END:
	           printf("INFO: END\n") ;
	           MPI_Comm_free(client) ;
	           return 0;

              case REQ_ACTION_DISCONNECT:
	           printf("INFO: Disconnect\n") ;
	           MPI_Comm_disconnect(client) ;
	           again = 0 ;
	           break;

              case REQ_ACTION_DATA:
	           printf("INFO: Received: %d\n", req_id) ;
	           break;

              default:
	           printf("ERROR: Unexpected message type: %d\n", req_action) ;
          }
    }

    th_dec(wb) ;
    return MPI_SUCCESS;
}


/*
 *  Main
 */

int main ( int argc, char **argv )
{
    server_stub_t wb ;
    int ret ;

    // Welcome...
    fprintf(stdout, "\n"
 		    " mfs_server (0.3)\n"
		    " ----------------\n"
		    "\n");

    // Initialize...
    fprintf(stdout, "INFO: Server initializing...\n") ;
    ret = serverstub_init(&wb, &argc, &argv) ;
    if (ret < 0) {
        fprintf(stderr, "ERROR: serverstub_init fails :-S") ;
        return -1 ;
    }

    // To serve requests...
    while (0 == wb.the_end)
    {
        serverstub_accept(&wb, do_srv) ;
    }

    // Finalize...
    fprintf(stdout, "INFO: Server[%d] ends.\n", wb.rank) ;
    serverstub_finalize(&wb) ;

    return 0 ;
}

