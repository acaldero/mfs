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

#ifndef __MFS_POOL_H__
#define __MFS_POOL_H__

   #include <stdio.h>
   #include <time.h>
   #include <pthread.h>
   #include "mfs_protocol.h"
   #include "mfs_lib.h"


   /*
    * Constants
    */

   #define MAX_BUFFER   128
   #define MAX_SERVICIO   5


   /*
    * Datatype
    */

   typedef struct st_th_args th_args_t;

   struct st_th_args
   {
        comm_t    ab ;
        void (*function)(th_args_t) ;
     // mfs_param_t *params;
   };


   /*
    * API
    */

   int  mfs_pool_init      ( void ) ;
   int  mfs_pool_launch    ( comm_t *wb, void (*worker_function)(th_args_t) ) ;
   int  mfs_pool_waitall   ( void ) ;

#endif

