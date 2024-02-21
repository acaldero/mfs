/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#ifndef __MFS_WORKER_ONREQUEST_H__
#define __MFS_WORKER_ONREQUEST_H__

    #include "base_lib.h"
    #include "mfs_worker.h"


    class mfs_worker_onrequest : mfs_worker
    {
        protected:
	   friend void *mfs_workers_onrequest_worker_run ( void *arg ) ;

        public:
           struct st_th last_st_th ;
           int sync_copied = 0 ;
           int n_workers   = 0 ;
           pthread_mutex_t m_worker ;
           pthread_cond_t  c_worker ;
           pthread_cond_t  end_cond ;
           params_t *or_params ;

            mfs_worker_onrequest ( ) ;
           ~mfs_worker_onrequest ( ) ;

           int  init          ( params_t *params ) ;
           int  launch_worker ( comm_t *wb, void (*worker_function)(struct st_th) ) ;
           int  destroy       ( void ) ;

           int  stats_show    ( char *prefix ) ;
    } ;


#endif

