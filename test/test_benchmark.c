
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

#define N_TIMES_BENCHMARK 10
#define N_SIZES_BENCHMARK 1024
#define BUFFER_SIZE 1024
char    buffer[BUFFER_SIZE] ;

int main_simple2 ( params_t *params )
{
    int    ret ;
    comm_t wb ;
    long   fd ;
    long   t1, t2 ;

    // Initialize...
    ret = clientstub_init(&wb, params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: clientstub_init fails :-(", -1) ;
        return -1 ;
    }

    // Benchmark: write
    memset(buffer, 'x', BUFFER_SIZE) ;

    printf("test;\t\tclient;\t\tsize (KiB);\tavg.time (seconds);\n") ;
    for (int j=1; j<N_SIZES_BENCHMARK; j=2*j)
    {
         t1 = mfs_get_time() ;
         for (int i=0; i<N_TIMES_BENCHMARK; i++)
         {
              fd = clientstub_open(&wb, "test1.txt", O_WRONLY | O_CREAT | O_TRUNC) ;
              for (int k=0; k<j; k++)
                   clientstub_write(&wb, fd, buffer, BUFFER_SIZE) ;
              clientstub_close(&wb, fd) ;
         }
         t2 = mfs_get_time() ;
         printf("write;\t\t%d;\t\t%d;\t\t%lf;\n", wb.rank, (j*BUFFER_SIZE)/1024, ((t2-t1)/1000.0)/N_TIMES_BENCHMARK) ;
    }

    // Benchmark: read
    printf("test;\t\tclient;\t\tsize (KiB);\tavg.time (seconds);\n") ;
    for (int j=1; j<N_SIZES_BENCHMARK; j=2*j)
    {
         t1 = mfs_get_time() ;
         for (int i=0; i<N_TIMES_BENCHMARK; i++)
         {
              fd = clientstub_open(&wb, "test1.txt", O_RDONLY) ;
              for (int k=0; k<j; k++)
                   clientstub_read( &wb, fd, buffer, BUFFER_SIZE) ;
              clientstub_close(&wb, fd) ;
         }
         t2 = mfs_get_time() ;
         printf("read;\t\t%d;\t\t%d;\t\t%lf;\n", wb.rank, (j*BUFFER_SIZE)/1024, ((t2-t1)/1000.0)/N_TIMES_BENCHMARK) ;
    }

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
    ret = main_simple2(&params) ;

    // Return OK/KO
    return ret ;
}

