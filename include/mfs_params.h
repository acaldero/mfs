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

#ifndef _MFS_PARAMS_H_
#define _MFS_PARAMS_H_

   #include <stdlib.h>
   #include <stdio.h>
   #include <string.h>
   #include <getopt.h>

   #include "mfs_lib.h"
   #include "mfs_files.h"
   #include "mfs_dbm.h"
   #include "mfs_directories.h"
   #include "mfs_comm_common.h"


   // Const
   #define MAX_NAME_LEN   1024

   #define DEFAULT_DATA_PREFIX "./data/"
   #define DEFAULT_STUB_PNAME  "mfs_server_v1"

   #define THREAD_USE_ONDEMAND  1
   #define THREAD_USE_POOL      2


   // Datatype
   typedef struct
   {
        // associated client
        char            data_prefix[MAX_NAME_LEN] ; // e.g.: "/mnt/mfs_root"
        char  mfs_server_stub_pname[MAX_NAME_LEN] ; // e.g.: "mfs_server_v1"
        int   num_servers ;

	// backend options
        int   file_backend ;
        char  file_backend_name[MAX_NAME_LEN] ;

        int   dbm_backend ;
        char  dbm_backend_name[MAX_NAME_LEN] ;

        int   directory_backend ;
        char  directory_backend_name[MAX_NAME_LEN] ;

        int   thread_launch ;
        char  thread_launch_name[MAX_NAME_LEN] ;

        int   comm_backend ;
        char  comm_backend_name[MAX_NAME_LEN] ;

        // server arguments
        int    *argc ;
        char ***argv ;

   } params_t ;


   /*
    * API
    */

   void mfs_params_show_usage ( void ) ;
   int  mfs_params_get        ( params_t *params, int *argc, char ***argv ) ;
   void mfs_params_show       ( params_t *params ) ;

#endif

