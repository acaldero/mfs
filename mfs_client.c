
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


int main ( int argc, char **argv )
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
    ret = clientstub_init(&wb, &argc, &argv) ;
    if (ret < 0) {
        fprintf(stderr, "ERROR: clientstub_init fails :-S") ;
        return -1 ;
    }

    // Requests...
    for (i = 0; i < 2; i++) {
         clientstub_request(&wb, REQ_ACTION_DATA, i) ;
    }
    clientstub_request(&wb, REQ_ACTION_DISCONNECT, i) ;

    // Finalize...
    clientstub_finalize(&wb) ;

    return 0;
}

