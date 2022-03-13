
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
#include "mfs_client_api.h"

#define N_TIMES_BENCHMARK 10
#define N_SIZES_BENCHMARK 2*1024
#define BUFFER_SIZE 1024
char    buffer[BUFFER_SIZE] ;

int main_simple2 ( params_t *params )
{
    int    ret ;
    comm_t wb ;
    long   fd ;
    long   kb, t1, t2 ;
    double mb, t ;

    // Initialize...
    ret = mfs_api_init(&wb, params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: mfs_api_init fails :-(", -1) ;
        return -1 ;
    }

    // Benchmark: write
    memset(buffer, 'x', BUFFER_SIZE) ;

    printf("protocol;\ttest;\t\tclient;\t\tsize (KiB);\tavg.bandwidth (MiB/sec.);\n") ;
    for (int j=1; j<N_SIZES_BENCHMARK; j=2*j)
    {
         t1 = mfs_get_time() ;
         for (int i=0; i<N_TIMES_BENCHMARK; i++)
         {
              fd = mfs_api_open(&wb, "test1.txt", O_RDWR | O_CREAT) ;
	      if (fd < 0) {
                  mfs_api_finalize(&wb) ;
		  exit(-1) ;
	      }

              for (int k=0; k<j; k++) {
                   mfs_api_write(&wb, fd, buffer, BUFFER_SIZE) ;
	      }
              mfs_api_close(&wb, fd) ;
         }
         t2 = mfs_get_time() ;
	 t  = (double) ((t2-t1)/1000.0) / N_TIMES_BENCHMARK ;
	 kb = (long)   (j*BUFFER_SIZE) / 1024 ;
	 mb = (double) kb / 1024 ;
         printf("%s\t\twrite;\t\t%d;\t\t%d;\t\t%lf;\n", params->file_backend_name, wb.rank, kb, mb/t) ;
    }

    // Benchmark: read
    printf("protocol;\ttest;\t\tclient;\t\tsize (KiB);\tavg.bandwidth (MiB/sec.);\n") ;
    for (int j=1; j<N_SIZES_BENCHMARK; j=2*j)
    {
         t1 = mfs_get_time() ;
         for (int i=0; i<N_TIMES_BENCHMARK; i++)
         {
              fd = mfs_api_open(&wb, "test1.txt", O_RDONLY) ;
	      if (fd < 0) {
                  mfs_api_finalize(&wb) ;
		  exit(-1) ;
	      }

              for (int k=0; k<j; k++) {
                   mfs_api_read( &wb, fd, buffer, BUFFER_SIZE) ;
	      }
              mfs_api_close(&wb, fd) ;
         }
         t2 = mfs_get_time() ;
	 t  = (double) ((t2-t1)/1000.0) / N_TIMES_BENCHMARK ;
	 kb = (long)   (j*BUFFER_SIZE) / 1024 ;
	 mb = (double) kb / 1024 ;
         printf("%s\t\tread;\t\t%d;\t\t%d;\t\t%lf;\n", params->file_backend_name, wb.rank, kb, mb/t) ;
    }

    // Finalize...
    mfs_print(DBG_INFO, "Client[%d]: finalize...\n", wb.rank) ;
    mfs_api_finalize(&wb) ;

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

