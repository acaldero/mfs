
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

#include "mfs_workers.h"


pool_t      th_pool ;

int mfs_workers_init ( params_t *params )
{
       int ret ;

       ret = mfs_workers_onrequest_init(params) ;
       if (ret < 0) {
	   return ret ;
       }

       ret = mfs_workers_pool_init(&th_pool, params) ;
       if (ret < 0) {
	   return ret ;
       }

       // Return OK
       return 1 ;
}

int mfs_workers_launch_worker ( int th_type, comm_t * wb, void (*worker_function)(struct st_th) )
{
       int ret ;

       switch (th_type)
       {
           case THREAD_USE_ONDEMAND:
                ret = mfs_workers_onrequest_launch_worker(wb, worker_function) ;
                break ;

           case THREAD_USE_POOL:
                ret = mfs_workers_pool_launch_worker(&th_pool, wb, worker_function) ;
                break ;

           default:
                mfs_print(DBG_INFO, "[FILE]: ERROR on th_type(%d).\n", th_type) ;
                return -1 ;
                break ;
       }

       // Return OK/KO
       return ret ;
}

int mfs_workers_destroy ( void )
{
       int ret ;

       ret = mfs_workers_onrequest_destroy() ;
       if (ret < 0) {
	   return ret ;
       }

       ret = mfs_workers_pool_destroy(&th_pool) ;
       if (ret < 0) {
	   return ret ;
       }

       // Return OK
       return 1 ;
}

int mfs_workers_stats_show ( char *prefix )
{
       int ret ;

       ret = mfs_workers_onrequest_stats_show(prefix) ;
       if (ret < 0) {
	   return ret ;
       }

       ret = mfs_workers_pool_stats_show(&th_pool, prefix) ;
       if (ret < 0) {
	   return ret ;
       }

       // Return OK
       return 1 ;
}

