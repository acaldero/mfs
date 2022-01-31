
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


#include <stdio.h>
#include "mfs_protocol.h"
#include "mfs_params.h"
#include "mfs_client_stub.h"


int main_simple1 ( params_t *params )
{
    int    ret ;
    comm_t wb ;
    long   fd ;
    #define STR_SIZE 1024
    char   str[STR_SIZE] ;

    // Initialize...
    ret = clientstub_init(&wb, params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: clientstub_init fails :-(", -1) ;
        return -1 ;
    }

    // Example
    strcpy(str, "hello word") ;

    mfs_print(DBG_INFO, "Client[%d]: creat(...) + write(...) + close(...)\n", wb.rank) ;
    fd = clientstub_open(&wb, "test1.txt", O_WRONLY | O_CREAT | O_TRUNC) ;
    clientstub_write(&wb, fd, str, strlen(str)) ;
    clientstub_close(&wb, fd) ;

    mfs_print(DBG_INFO, "Client[%d]: open(...) + read(...) + close(...)\n", wb.rank) ;
    fd = clientstub_open(&wb, "test1.txt", O_RDONLY) ;
    clientstub_read( &wb, fd, str, STR_SIZE) ;
    clientstub_close(&wb, fd) ;

    // Finalize...
    mfs_print(DBG_INFO, "Client[%d]: finalize...\n", wb.rank) ;
    clientstub_finalize(&wb) ;

    return 0;
}

int main ( int argc, char **argv )
{
    int  ret ;
    params_t params ;

    // Welcome...
    printf("\n"
 	   " mfs_client\n"
	   " ----------\n"
	   "\n") ;

    // Get parameters..
    ret = mfs_params_get(&params, &argc, &argv) ;
    if (ret < 0) {
        mfs_params_show_usage() ;
        exit(-1) ;
    }

    mfs_print(DBG_INFO, "Client[%d]: initializing...\n", -1) ;
    mfs_params_show(&params) ;

    // simple main...
    ret = main_simple1(&params) ;

    // Return OK/KO
    return ret ;
}

