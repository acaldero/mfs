
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


#include "mfs_client_stub.h"

#define STR_SIZE 1024

int main ( int argc, char **argv )
{
    client_stub_t wb ;
    int ret ;
    int i, fd ;
    char str[STR_SIZE] ;

    // Welcome...
    fprintf(stdout, "\n"
 		    " mfs_client\n"
		    " ----------\n"
		    "\n");

    // Initialize...
    mfs_print(stdout, "INFO: Client initializing...\n") ;
    ret = clientstub_init(&wb, &argc, &argv) ;
    if (ret < 0) {
        mfs_print(stderr, "ERROR: clientstub_init fails :-S") ;
        return -1 ;
    }

    // Example
    strcpy(str, "hello word") ;

    fd = clientstub_open(&wb, "test1.txt", O_WRONLY | O_CREAT | O_TRUNC) ;
    clientstub_write(&wb, fd, str, strlen(str)) ;
    clientstub_close(&wb, fd) ;

    fd = clientstub_open(&wb, "test1.txt", O_RDONLY) ;
    clientstub_read( &wb, fd, str, STR_SIZE) ;
    clientstub_close(&wb, fd) ;

    // Finalize...
    clientstub_finalize(&wb) ;

    return 0;
}

