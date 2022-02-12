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

#ifndef __MFS_WORKERS_POOL_H__
#define __MFS_WORKERS_POOL_H__

   #include <pthread.h>
   #include "mfs_lib.h"
   #include "mfs_protocol.h"
   #include "mfs_params.h"


   /*
    * Constants
    */

   #define POOL_MAX_REQUESTS  128


   /*
    * Datatype
    */

   struct pool_t
   {
	pthread_t    *pool_ths ;
	struct st_th *pool_buffer ;

	int  pool_n_eltos ;
	int  is_running ;
	int  pool_theend ;
	int  buff_position_receptor ;
	int  buff_position_service ;
	int  POOL_MAX_THREADS ;
	params_t *params ;

	pthread_mutex_t  mutex ;
	pthread_cond_t   c_no_full ;
	pthread_cond_t   c_no_empty ;
	pthread_cond_t   c_running ;
	pthread_cond_t   c_stopped ;
   };


   /*
    * API
    */

   int  mfs_workers_pool_init           ( pool_t *thpool, params_t *params ) ;
   int  mfs_workers_pool_launch_worker  ( pool_t *thpool, comm_t *wb, void (*worker_function)(struct st_th) ) ;
   int  mfs_workers_pool_wait_workers   ( pool_t *thpool ) ;

   int  mfs_workers_pool_stats_show     ( pool_t *thpool, char *prefix ) ;

#endif

