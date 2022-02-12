
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
#include "mfs_workers_pool.h"


/*
 * Auxiliar funciton
 */

void * th_pool_service ( void * param )
{
      struct st_th  p ;
      pool_t    *th_pool ;

      // signal initializate...
      th_pool = (pool_t *)param ;
      pthread_mutex_lock(&(th_pool->mutex)) ;
      th_pool->is_running = 1 ;
      pthread_cond_signal(&(th_pool->c_running)) ;
      pthread_mutex_unlock(&(th_pool->mutex)) ;

      while (0 == th_pool->pool_theend)
      {
	   // lock when not empty and not ended...
           pthread_mutex_lock(&(th_pool->mutex)) ;
           while (th_pool->pool_n_eltos == 0)
	   {
                if (1 == th_pool->pool_theend) {
                     pthread_cond_signal(&(th_pool->c_stopped)) ;
                     pthread_mutex_unlock(&(th_pool->mutex)) ;
                     pthread_exit(0);
                }

                pthread_cond_wait(&(th_pool->c_no_empty), &(th_pool->mutex)) ;
           } // while

	   // removing element from buffer...
           memmove(&p, &(th_pool->pool_buffer[th_pool->buff_position_service]), sizeof(struct st_th)) ;
           th_pool->buff_position_service = (th_pool->buff_position_service + 1) % POOL_MAX_REQUESTS ;
           th_pool->pool_n_eltos--;

	   // signal not full...
           pthread_cond_signal(&(th_pool->c_no_full)) ;
           pthread_mutex_unlock(&(th_pool->mutex)) ;

	   // process and response...
	   p.function(p) ;
    }

    pthread_exit(0);
    return NULL;
}


/*
 *  API
 */

int  mfs_workers_pool_init ( pool_t *th_pool, params_t *params )
{
    // check arguments...
    if (NULL == th_pool) {
	return -1 ;
    }

    // initialize
    th_pool->params = params ;
    pthread_mutex_init(&(th_pool->mutex),      NULL) ;
    pthread_cond_init (&(th_pool->c_no_full),  NULL) ;
    pthread_cond_init (&(th_pool->c_no_empty), NULL) ;
    pthread_cond_init (&(th_pool->c_running),  NULL) ;
    pthread_cond_init (&(th_pool->c_stopped),  NULL) ;

    // malloc requests
    th_pool->pool_buffer = (struct st_th *)malloc(POOL_MAX_REQUESTS * sizeof(struct st_th)) ;
    if (NULL == th_pool->pool_buffer) {
	return -1 ;
    }

    // malloc threads
    th_pool->POOL_MAX_THREADS = sysconf(_SC_NPROCESSORS_ONLN) ;

    th_pool->pool_ths = (pthread_t *)malloc(th_pool->POOL_MAX_THREADS * sizeof(pthread_t)) ;
    if (NULL == th_pool->pool_ths) {
	return -1 ;
    }

    // starting threads
    for (int i=0; i<th_pool->POOL_MAX_THREADS; i++)
    {
	  // create thread
          pthread_create(&(th_pool->pool_ths[i]), NULL, th_pool_service, th_pool) ;

          // wait for thread being started
          pthread_mutex_lock(&(th_pool->mutex)) ;
	  while (0 == th_pool->is_running) {
                 pthread_cond_wait(&(th_pool->c_running), &(th_pool->mutex)) ;
	  }
          th_pool->is_running = 0 ;
          pthread_mutex_unlock(&(th_pool->mutex)) ;
    }

    // return OK
    return 1;
}


int  mfs_workers_pool_launch_worker  ( pool_t *th_pool, comm_t *wb, void (*worker_function)(struct st_th) )
{
    // Check params...
    if (NULL == th_pool) {
        return -1 ;
    }

    // lock when not full...
    pthread_mutex_lock(&(th_pool->mutex)) ;
    while (th_pool->pool_n_eltos == POOL_MAX_REQUESTS) {
           pthread_cond_wait(&(th_pool->c_no_full), &(th_pool->mutex)) ;
    }

    // inserting element into the buffer
    th_pool->pool_buffer[th_pool->buff_position_receptor].ab       = *wb ;
    th_pool->pool_buffer[th_pool->buff_position_receptor].function = worker_function ;
    th_pool->buff_position_receptor = (th_pool->buff_position_receptor +1) % POOL_MAX_REQUESTS;
    th_pool->pool_n_eltos++;

    // signal not empty...
    pthread_cond_signal(&(th_pool->c_no_empty)) ;
    pthread_mutex_unlock(&(th_pool->mutex)) ;

    // return OK
    return 1;
}

int  mfs_workers_pool_wait_workers ( pool_t *th_pool )
{
    // Check params...
    if (NULL == th_pool) {
        return -1 ;
    }

    // finalizar
    th_pool->pool_theend = 1 ;
    pthread_cond_broadcast(&(th_pool->c_no_empty)) ;

    for (int i=0; i<th_pool->POOL_MAX_THREADS; i++) {
         pthread_join(th_pool->pool_ths[i], NULL);
    }

    // free
    free(th_pool->pool_buffer) ;
    free(th_pool->pool_ths) ;

    // destroy
    pthread_mutex_destroy(&(th_pool->mutex)) ;
    pthread_cond_destroy (&(th_pool->c_no_full)) ;
    pthread_cond_destroy (&(th_pool->c_no_empty)) ;
    pthread_cond_destroy (&(th_pool->c_running)) ;
    pthread_cond_destroy (&(th_pool->c_stopped)) ;

    // return OK
    return 1;
}

int mfs_workers_pool_stats_show ( pool_t *th_pool, char *prefix )
{
    // Check params...
    if (NULL == th_pool) {
        return -1 ;
    }
    if (NULL == prefix) {
        return -1 ;
    }

    // Print stats...
    printf("%s: Threads (pool):\n",       prefix) ;
    printf("%s: + max. threads=%ld\n",    prefix, th_pool->POOL_MAX_THREADS) ;
    printf("%s: + max. requests=%ld\n",   prefix, POOL_MAX_REQUESTS) ;
    printf("%s: + active threads=%ld\n",  prefix, th_pool->pool_n_eltos) ;

    // Return OK
    return 1 ;
}

