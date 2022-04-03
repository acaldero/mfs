
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of XPNlite.
 *
 *  XPNlite is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPNlite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with XPNlite.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <stdio.h>
#include "mfs_api.h"

#define N_TIMES_BENCHMARK     10
#define N_SIZES_BENCHMARK 2*1024
#define BUFFER_SIZE         1024

char buffer[BUFFER_SIZE] ;

int main_benchmark1 ( mfs_t *wb, params_t *params )
{
    int    ret, rank ;
    long   fd ;
    long   kb, t1, t2 ;
    double mb, t ;

    // Initialize...
    ret = mfs_api_open_partition(wb, params, params->conf_fname) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: mfs_api_open_partition fails :-(\n", -1) ;
        return -1 ;
    }

    rank = mfs_api_get_rank(wb) ;
    memset(buffer, 'x', BUFFER_SIZE) ;

    // Benchmark: write
    printf("protocol;\ttest;\t\tclient;\t\tsize (KiB);\tavg.bandwidth (MiB/sec.);\n") ;
    for (int j=1; j<N_SIZES_BENCHMARK; j=2*j)
    {
         t1 = mfs_get_time() ;
         for (int i=0; i<N_TIMES_BENCHMARK; i++)
         {
              fd = mfs_api_open(wb, "test1.txt", O_RDWR | O_CREAT) ;
	      if (fd < 0) { return -1 ; }

              for (int k=0; k<j; k++) {
                   mfs_api_write(wb, fd, buffer, BUFFER_SIZE) ;
	      }
              mfs_api_close(wb, fd) ;
         }
         t2 = mfs_get_time() ;
	 t  = (double) ((t2-t1)/1000.0) / N_TIMES_BENCHMARK ;
	 kb = (long)   (j*BUFFER_SIZE) / 1024 ;
	 mb = (double) kb / 1024 ;
         printf("%s\t\twrite;\t\t%d;\t\t%d;\t\t%lf;\n", params->file_backend_name, rank, kb, mb/t) ;
    }

    // Benchmark: read
    printf("protocol;\ttest;\t\tclient;\t\tsize (KiB);\tavg.bandwidth (MiB/sec.);\n") ;
    for (int j=1; j<N_SIZES_BENCHMARK; j=2*j)
    {
         t1 = mfs_get_time() ;
         for (int i=0; i<N_TIMES_BENCHMARK; i++)
         {
              fd = mfs_api_open(wb, "test1.txt", O_RDONLY) ;
	      if (fd < 0) { return -1 ; }

              for (int k=0; k<j; k++) {
                   mfs_api_read(wb, fd, buffer, BUFFER_SIZE) ;
	      }
              mfs_api_close(wb, fd) ;
         }
         t2 = mfs_get_time() ;
	 t  = (double) ((t2-t1)/1000.0) / N_TIMES_BENCHMARK ;
	 kb = (long)   (j*BUFFER_SIZE) / 1024 ;
	 mb = (double) kb / 1024 ;
         printf("%s\t\tread;\t\t%d;\t\t%d;\t\t%lf;\n", params->file_backend_name, rank, kb, mb/t) ;
    }

    mfs_api_close_partition(wb, params) ;

    return 0;
}

int main ( int argc, char **argv )
{
    int      ret ;
    mfs_t    wb ;
    params_t params ;

    // Welcome...
    printf("\n"
 	   " mfs_client\n"
	   " ----------\n"
	   "\n") ;

    // Initialize...
    mfs_print(DBG_INFO, "Client[%d]: initializing...\n", -1) ;
    ret = mfs_api_init(&wb, &params, &argc, &argv) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: mfs_api_init fails :-(\n", -1) ;
        exit(-1) ;
    }

    // simple main...
    ret = main_benchmark1(&wb, &params) ;

    // Finalize...
    mfs_print(DBG_INFO, "Client[%d]: finalize...\n", -1) ;
    mfs_api_finalize(&wb, &params) ;

    // Return OK/KO
    return ret ;
}

