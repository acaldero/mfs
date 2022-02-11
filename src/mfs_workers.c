
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


int mfs_workers_init ( void )
{
       int ret ;

       ret = mfs_workers_onrequest_init() ;
    // ret = mfs_workers_pool_init() ;

       // Return OK/KO
       return ret;
}

int mfs_workers_launch_worker ( comm_t * wb, void (*worker_function)(struct st_th) )
{
       int ret ;

       ret = mfs_workers_onrequest_launch_worker(wb, worker_function) ;
    // ret = mfs_workers_pool_launch(wb, worker_function) ;

       // Return OK/KO
       return ret;
}

int mfs_workers_wait_workers ( void )
{
       int ret ;

       ret = mfs_workers_onrequest_wait_workers() ;
    // ret = mfs_workers_pool_waitall() ;

       // Return OK/KO
       return ret;
}

int mfs_workers_stats_show ( char *prefix )
{
       int ret ;

       ret = mfs_workers_onrequest_stats_show(prefix) ;
    // ret = mfs_workers_pool_stats_show(prefix) ;

       // Return OK/KO
       return ret;
}

