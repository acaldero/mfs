
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

#include "mfs_workers_pool.h"


/*
 *  Internal
 */

th_args_t  *pool_buffer ;
pthread_t  *pool_ths ;

int  pool_n_eltos = 0 ;
int  is_running   = 0 ;
int  pool_theend  = 0 ;
int  buff_position_receptor = 0 ;
int  buff_position_service  = 0 ;
int  POOL_MAX_THREADS = 2 ;
params_t *pool_params ;

pthread_mutex_t  mutex ;
pthread_cond_t   c_no_full ;
pthread_cond_t   c_no_empty ;
pthread_cond_t   c_running ;
pthread_cond_t   c_stopped ;


void * th_pool_service ( void * param )
{
      th_args_t p;

      // signal initializate...
      pthread_mutex_lock(&mutex) ;
      is_running = 1 ;
      pthread_cond_signal(&c_running) ;
      pthread_mutex_unlock(&mutex) ;

      while (0 == pool_theend)
      {
	   // lock when not empty and not ended...
           pthread_mutex_lock(&mutex);
           while (0 == pool_n_eltos)
	   {
                if (1 == pool_theend) {
                     pthread_cond_signal(&c_stopped) ;
                     pthread_mutex_unlock(&mutex) ;
                     pthread_exit(0) ;
                }

                pthread_cond_wait(&c_no_empty, &mutex);
           } // while

	   // removing element from buffer...
           memmove(&p, &(pool_buffer[buff_position_service]), sizeof(th_args_t)) ;
           buff_position_service = (buff_position_service + 1) % POOL_MAX_REQUESTS ;
           pool_n_eltos--;

	   // signal not full...
           pthread_cond_signal(&c_no_full) ;
           pthread_mutex_unlock(&mutex) ;

	   // process and response...
	   p.function(p) ;
    }

    pthread_exit(0);
    return NULL;
}


/*
 *  API
 */

int  mfs_workers_pool_init ( params_t *params )
{
    // initialize
    pthread_mutex_init(&mutex,      NULL) ;
    pthread_cond_init (&c_no_full,  NULL) ;
    pthread_cond_init (&c_no_empty, NULL) ;
    pthread_cond_init (&c_running,  NULL) ;
    pthread_cond_init (&c_stopped,  NULL) ;
    pool_params = params ;

    // get number of cores
    POOL_MAX_THREADS = sysconf(_SC_NPROCESSORS_ONLN) ;

    // malloc
    pool_buffer = (th_args_t *)malloc(POOL_MAX_REQUESTS * sizeof(th_args_t)) ;
    if (NULL == pool_buffer) {
	return -1 ;
    }

    pool_ths = (pthread_t *)malloc(POOL_MAX_THREADS * sizeof(pthread_t)) ;
    if (NULL == pool_ths) {
	return -1 ;
    }

    // starting threads
    for (int i=0; i<POOL_MAX_THREADS; i++)
    {
	  // create thread
          pthread_create(&(pool_ths[i]), NULL, th_pool_service, NULL) ;

          // wait for thread being started
          pthread_mutex_lock(&mutex) ;
	  while (0 == is_running) {
                 pthread_cond_wait(&c_running, &mutex) ;
	  }
          is_running = 0 ;
          pthread_mutex_unlock(&mutex) ;
    }

    // return OK
    return 1;
}


int  mfs_workers_pool_launch_worker  ( comm_t *wb, void (*worker_function)(th_args_t) )
{
    // lock when not full...
    pthread_mutex_lock(&mutex);
    while (pool_n_eltos == POOL_MAX_REQUESTS) {
           pthread_cond_wait(&c_no_full, &mutex);
    }

    // inserting element into the buffer
    pool_buffer[buff_position_receptor].ab       = *wb ;
    pool_buffer[buff_position_receptor].function = worker_function ;
    buff_position_receptor = (buff_position_receptor +1) % POOL_MAX_REQUESTS;
    pool_n_eltos++;

    // signal not empty...
    pthread_cond_signal(&c_no_empty);
    pthread_mutex_unlock(&mutex);

    // return OK
    return 1;
}

int  mfs_workers_pool_wait_workers ( void )
{
    // finalizar
    pool_theend = 1;
    pthread_cond_broadcast(&c_no_empty) ;

    for (int i=0; i<POOL_MAX_THREADS; i++) {
         pthread_join(pool_ths[i], NULL) ;
    }

    // free
    free(pool_buffer) ;
    free(pool_ths) ;

    // destroy
    pthread_mutex_destroy(&mutex) ;
    pthread_cond_destroy(&c_no_full) ;
    pthread_cond_destroy(&c_no_empty) ;
    pthread_cond_destroy(&c_running) ;
    pthread_cond_destroy(&c_stopped) ;

    // return OK
    return 1;
}

int mfs_workers_pool_stats_show ( char *prefix )
{
    // Check params...
    if (NULL == prefix) {
        return -1 ;
    }

    // Print stats...
    printf("%s: Threads (pool):\n",       prefix) ;
    printf("%s: + max. threads=%ld\n",    prefix, POOL_MAX_THREADS) ;
    printf("%s: + max. requests=%ld\n",   prefix, POOL_MAX_REQUESTS) ;
    printf("%s: + active threads=%ld\n",  prefix, pool_n_eltos) ;

    // Return OK
    return 1 ;
}

