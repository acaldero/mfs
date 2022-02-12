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

#ifndef __MFS_WORKERS_ONREQUEST_H__
#define __MFS_WORKERS_ONREQUEST_H__

   #include "mfs_workers.h"
   #include "mfs_lib.h"


   /*
    * API
    */

   int  mfs_workers_onrequest_init          ( params_t *params ) ;
   int  mfs_workers_onrequest_launch_worker ( comm_t *wb, void (*worker_function)(struct st_th) ) ;
   int  mfs_workers_onrequest_wait_workers  ( void ) ;

   int  mfs_workers_onrequest_stats_show    ( char *prefix ) ;

#endif

