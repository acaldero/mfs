
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

#include "mfs_workers_onrequest.h"


/*
 *  Internal
 */

int sync_copied = 0 ;
int n_workers   = 0 ;
pthread_mutex_t m_worker ;
pthread_cond_t  c_worker ;
pthread_cond_t  end_cond ;
params_t *or_params ;

void *mfs_workers_onrequest_worker_run ( void *arg )
{
       struct st_th th;

       // prolog...
       pthread_mutex_lock(&m_worker);
       memcpy(&th, arg, sizeof(struct st_th)) ;
       n_workers++ ;
       sync_copied = 1 ;
       pthread_cond_broadcast(&c_worker); // pthread_cond_signal(&c_worker);
       pthread_mutex_unlock(&m_worker);

       // do function code...
       th.function(th) ;

       // epilog...
       pthread_mutex_lock(&m_worker) ;
       n_workers-- ;
       if (0 == n_workers) {
           pthread_cond_signal(&end_cond) ; // pthread_cond_signal(&end_cond);
       }
       pthread_mutex_unlock(&m_worker) ;

       // end
       pthread_exit(0);
       return NULL ;
}


/*
 * API
 */

int mfs_workers_onrequest_init ( params_t *params )
{
       n_workers   = 0 ;
       sync_copied = 0 ;
       pthread_mutex_init(&m_worker, NULL) ;
       pthread_cond_init (&c_worker, NULL) ;
       pthread_cond_init (&end_cond, NULL) ;
       or_params   = params ;

       // Return OK
       return 1;
}

int mfs_workers_onrequest_launch_worker ( comm_t * wb, void (*worker_function)(struct st_th) )
{
       int            ret;
       pthread_attr_t th_attr;
       pthread_t      th_worker;
       struct st_th   st_worker;

       pthread_attr_init(&th_attr);
       pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED);
       pthread_attr_setstacksize  (&th_attr, STACK_SIZE);
       sync_copied = 0 ;

       // prepare arguments...
       st_worker.ab       = *wb ;
       st_worker.function = worker_function ;

       // create thread...
       ret = pthread_create(&th_worker, &th_attr, (mfs_workers_onrequest_worker_run), (void *)&st_worker);
       if (ret != 0){
           mfs_print(DBG_ERROR, "[WORKERS]: pthread_create fails :-(\n") ;
           perror("pthread_create: ");
       }

       // wait to copy arguments + active_thread++...
       pthread_mutex_lock(&m_worker);
       while (sync_copied == 0) {
              pthread_cond_wait(&c_worker, &m_worker) ;
       }
       sync_copied = 0 ;
       pthread_mutex_unlock(&m_worker);

       // Return OK
       return 1;
}

int mfs_workers_onrequest_destroy ( void )
{
       // wait to n_workers be zero...
       pthread_mutex_lock(&m_worker) ;
       while (n_workers != 0) {
              pthread_cond_wait(&end_cond, &m_worker) ;
       }
       pthread_mutex_unlock(&m_worker) ;

       // destroy
       pthread_mutex_destroy(&m_worker) ;
       pthread_cond_destroy (&c_worker) ;
       pthread_cond_destroy (&end_cond) ;

       // Return OK
       return 1;
}

int mfs_workers_onrequest_stats_show ( char *prefix )
{
    // Check params...
    if (NULL == prefix) {
        return -1 ;
    }

    // Print stats...
    printf("%s: Threads (on request):\n", prefix) ;
    printf("%s: + active threads=%ld\n",  prefix, n_workers) ;

    // Return OK
    return 1 ;
}

