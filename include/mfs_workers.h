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

#ifndef __MFS_WORKERS_H__
#define __MFS_WORKERS_H__

   #include "mfs_workers_common.h"
   #include "mfs_workers_onrequest.h"
   #include "mfs_workers_pool.h"


   // API
   int  mfs_workers_init          ( params_t *params ) ;
   int  mfs_workers_launch_worker ( int th_type, comm_t * wb, void (*worker_function)(struct st_th) ) ;
   int  mfs_workers_destroy       ( void ) ;

   int  mfs_workers_stats_show    ( char *prefix ) ;

#endif

