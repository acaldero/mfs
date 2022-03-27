/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MFS.
 *
 *  MFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _MFS_PARAMS_H_
#define _MFS_PARAMS_H_

   #include "base_lib.h"
   #include "mfs_files.h"
   #include "mfs_dbm.h"
   #include "mfs_directories.h"
   #include "mfs_comm_common.h"
   #include "info_ns_common.h"

   #include <getopt.h>


   // Const
   #define MAX_NAME_LEN   1024

   #define DEFAULT_VERBOSE_LEVEL   0
   #define DEFAULT_DATA_PREFIX     "./data"
   #define DEFAULT_CONF_FILE       "conf.yaml"
   #define DEFAULT_STUB_PNAME      "mfs"
   #define DEFAULT_PORT            12345

   #define THREAD_USE_ONDEMAND  1
   #define THREAD_USE_POOL      2


   // Datatype
   typedef struct
   {
        int    verbose ;

        // associated client
        char  *mfs_server_stub_pname ; // e.g.: "mfs"
        char            *data_prefix ; // e.g.: "/mnt/mfs"
        char             *conf_fname ; // e.g.: "/etc/mfs/conf.yaml"
	int              server_port ; // e.g.: 12345

	// backend options
        int    file_backend ;
        char  *file_backend_name ;

        int    dbm_backend ;
        char  *dbm_backend_name ;

        int    directory_backend ;
        char  *directory_backend_name ;

        int    thread_launch ;
        char  *thread_launch_name ;

        int    comm_backend ;
        char  *comm_backend_name ;

        int    ns_backend ;
        char  *ns_backend_name ;

        // server arguments
        int    *argc ;
        char ***argv ;

   } params_t ;


   /*
    * API
    */

   void info_params_show_usage ( void ) ;
   int  info_params_get        ( params_t *params, int *argc, char ***argv ) ;
   int  info_params_show       ( params_t *params ) ;
   int  info_params_free       ( params_t *params ) ;

#endif

