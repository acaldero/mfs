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

#ifndef __MFS_WORKER_POOL_H__
#define __MFS_WORKER_POOL_H__

   #include "base_lib.h"
   #include "mfs_worker.h"


   /*
    * Constants
    */

   #define POOL_MAX_REQUESTS  128


   /*
    * Datatype
    */

    class mfs_worker_pool : mfs_worker
    {
        protected:
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

	   friend void * th_pool_service ( void * param ) ;

        public:
            mfs_worker_pool ( ) ;
           ~mfs_worker_pool ( ) ;

           int  init           ( params_t *params ) ;
           int  launch_worker  ( comm_t *wb, void (*worker_function)(struct st_th) ) ;
           int  destroy        ( void ) ;

           int  stats_show     ( char *prefix ) ;
    } ;


#endif

