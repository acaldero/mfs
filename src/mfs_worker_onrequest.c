
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

#include "mfs_worker_onrequest.h"


/*
 *  Internal
 */

void *mfs_worker_onrequest_worker_run ( void *arg )
{
       mfs_worker_onrequest th;

       // prolog...
       memcpy(&th, arg, sizeof(mfs_worker_onrequest)) ;

       pthread_mutex_lock(&(th.m_worker));
       th.n_workers++ ;
       th.sync_copied = 1 ;
       pthread_cond_broadcast(&(th.c_worker)); // pthread_cond_signal(&(th.c_worker));
       pthread_mutex_unlock(&(th.m_worker));

       // do function code...
       th.last_st_th.function(th.last_st_th) ;

       // epilog...
       pthread_mutex_lock(&(th.m_worker)) ;
       th.n_workers-- ;
       if (0 == th.n_workers) {
           pthread_cond_signal(&(th.end_cond)) ; // pthread_cond_signal(&(th.end_cond));
       }
       pthread_mutex_unlock(&(th.m_worker)) ;

       // end
       pthread_exit(0);
       return NULL ;
}


/*
 * API
 */

mfs_worker_onrequest::mfs_worker_onrequest ( )
{
       this->n_workers   = 0 ;
       this->sync_copied = 0 ;

       pthread_mutex_init(&(this->m_worker), NULL) ;
       pthread_cond_init (&(this->c_worker), NULL) ;
       pthread_cond_init (&(this->end_cond), NULL) ;

       this->worker_backend = 2 ;
       this->worker_backend_name = "ON-DEMAND" ;
}

mfs_worker_onrequest::~mfs_worker_onrequest ( )
{
       // wait to n_workers be zero...
       pthread_mutex_lock(&(this->m_worker)) ;
       while (this->n_workers != 0) {
              pthread_cond_wait(&(this->end_cond), &(this->m_worker)) ;
       }
       pthread_mutex_unlock(&(this->m_worker)) ;

       // destroy
       pthread_mutex_destroy(&(this->m_worker)) ;
       pthread_cond_destroy (&(this->c_worker)) ;
       pthread_cond_destroy (&(this->end_cond)) ;
}

int mfs_worker_onrequest::init ( params_t *params )
{
       this->or_params = params ;

       // Return OK
       return 1;
}

int mfs_worker_onrequest::destroy ( void )
{
       // wait to n_workers be zero...
       pthread_mutex_lock(&(this->m_worker)) ;
       while (this->n_workers != 0) {
              pthread_cond_wait(&(this->end_cond), &(this->m_worker)) ;
       }
       pthread_mutex_unlock(&(this->m_worker)) ;

       // Return OK
       return 1;
}

int mfs_worker_onrequest::launch_worker ( comm_t * wb, void (*worker_function)(struct st_th) )
{
       int            ret;
       pthread_attr_t th_attr;
       pthread_t      th_worker;

       pthread_attr_init(&th_attr);
       pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED);
       pthread_attr_setstacksize  (&th_attr, STACK_SIZE);

       // prepare arguments...
       this->sync_copied         = 0 ;
       this->last_st_th.ab       = *wb ;
       this->last_st_th.function = worker_function ;

       // create thread...
       ret = pthread_create(&th_worker, &th_attr, (mfs_worker_onrequest_worker_run), (void *)this);
       if (ret != 0){
           mfs_print(DBG_ERROR, "[WORKERS]: pthread_create fails :-(\n") ;
           perror("pthread_create: ");
       }

       // wait to copy arguments + active_thread++...
       pthread_mutex_lock(&(this->m_worker));
       while (this->sync_copied == 0) {
              pthread_cond_wait(&(this->c_worker), &(this->m_worker)) ;
       }
       this->sync_copied = 0 ;
       pthread_mutex_unlock(&(this->m_worker));

       // Return OK
       return 1;
}

int mfs_worker_onrequest::stats_show ( char *prefix )
{
    // Check params...
    if (NULL == prefix) {
        return -1 ;
    }

    // Print stats...
    printf("%s: Threads (on request):\n", prefix) ;
    printf("%s: + active threads=%ld\n",  prefix, this->n_workers) ;

    // Return OK
    return 1 ;
}


