
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

#include "mfs_worker_pool.h"


/*
 * Auxiliar function
 */

void * th_pool_service ( void * param )
{
      struct st_th  p ;
      mfs_worker_pool   *th_pool ;

      // signal initializate...
      th_pool = (mfs_worker_pool *)param ;

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

mfs_worker_pool::mfs_worker_pool ( )
{
    // initialize
    pthread_mutex_init(&(this->mutex),      NULL) ;
    pthread_cond_init (&(this->c_no_full),  NULL) ;
    pthread_cond_init (&(this->c_no_empty), NULL) ;
    pthread_cond_init (&(this->c_running),  NULL) ;
    pthread_cond_init (&(this->c_stopped),  NULL) ;

    this->params = NULL ;
    this->pool_buffer = NULL ;
    this->POOL_MAX_THREADS = 0 ;
    this->pool_ths = NULL ;

    this->worker_backend = 2 ;
    this->worker_backend_name = "POOL" ;
}

mfs_worker_pool::~mfs_worker_pool ( )
{
    // finalizar
    if (this->pool_theend != 1)
    {
        this->pool_theend = 1 ;
        pthread_cond_broadcast(&(this->c_no_empty)) ;

        for (int i=0; i<this->POOL_MAX_THREADS; i++) {
             pthread_join(this->pool_ths[i], NULL);
        }
    }

    // free
    if (this->pool_buffer != NULL) {
        free(this->pool_buffer) ;
        this->pool_buffer = NULL ;
    }

    if (this->pool_ths != NULL) {
        free(this->pool_ths) ;
        this->pool_ths = NULL ;
    }

    // destroy
    pthread_mutex_destroy(&(this->mutex)) ;
    pthread_cond_destroy (&(this->c_no_full)) ;
    pthread_cond_destroy (&(this->c_no_empty)) ;
    pthread_cond_destroy (&(this->c_running)) ;
    pthread_cond_destroy (&(this->c_stopped)) ;
}

int  mfs_worker_pool::init ( params_t *params )
{
    // check arguments...
    if (NULL == params) {
	return -1 ;
    }

    // initialize
    this->params = params ;

    // malloc requests
    this->pool_buffer = (struct st_th *)malloc(POOL_MAX_REQUESTS * sizeof(struct st_th)) ;
    if (NULL == this->pool_buffer) {
	return -1 ;
    }

    // malloc threads
    this->POOL_MAX_THREADS = sysconf(_SC_NPROCESSORS_ONLN) ;

    this->pool_ths = (pthread_t *)malloc(this->POOL_MAX_THREADS * sizeof(pthread_t)) ;
    if (NULL == this->pool_ths) {
	return -1 ;
    }

    // starting threads
    for (int i=0; i<this->POOL_MAX_THREADS; i++)
    {
	  // create thread
          pthread_create(&(this->pool_ths[i]), NULL, th_pool_service, this) ;

          // wait for thread being started
          pthread_mutex_lock(&(this->mutex)) ;
	  while (0 == this->is_running) {
                 pthread_cond_wait(&(this->c_running), &(this->mutex)) ;
	  }
          this->is_running = 0 ;
          pthread_mutex_unlock(&(this->mutex)) ;
    }

    this->worker_backend = 2 ;
    this->worker_backend_name = "POOL" ;

    // return OK
    return 1;
}

int  mfs_worker_pool::destroy ( void )
{
    // finalizar
    if (this->pool_theend != 1)
    {
        this->pool_theend = 1 ;
        pthread_cond_broadcast(&(this->c_no_empty)) ;

        for (int i=0; i<this->POOL_MAX_THREADS; i++) {
             pthread_join(this->pool_ths[i], NULL);
        }
    }

    // free
    if (this->pool_buffer != NULL) {
        free(this->pool_buffer) ;
        this->pool_buffer = NULL ;
    }

    if (this->pool_ths != NULL) {
        free(this->pool_ths) ;
        this->pool_ths = NULL ;
    }

    // return OK
    return 1;
}

int  mfs_worker_pool::launch_worker  ( comm_t *wb, void (*worker_function)(struct st_th) )
{
    // Check params...
    if (NULL == worker_function) {
        return -1 ;
    }

    // lock when not full...
    pthread_mutex_lock(&(this->mutex)) ;
    while (this->pool_n_eltos == POOL_MAX_REQUESTS) {
           pthread_cond_wait(&(this->c_no_full), &(this->mutex)) ;
    }

    // inserting element into the buffer
    this->pool_buffer[this->buff_position_receptor].ab       = *wb ;
    this->pool_buffer[this->buff_position_receptor].function = worker_function ;
    this->buff_position_receptor = (this->buff_position_receptor +1) % POOL_MAX_REQUESTS;
    this->pool_n_eltos++;

    // signal not empty...
    pthread_cond_signal(&(this->c_no_empty)) ;
    pthread_mutex_unlock(&(this->mutex)) ;

    // return OK
    return 1;
}

int mfs_worker_pool::stats_show ( char *prefix )
{
    // Check params...
    if (NULL == prefix) {
        return -1 ;
    }

    // Print stats...
    printf("%s: Threads (pool):\n",       prefix) ;
    printf("%s: + max. threads=%ld\n",    prefix, this->POOL_MAX_THREADS) ;
    printf("%s: + max. requests=%ld\n",   prefix, POOL_MAX_REQUESTS) ;
    printf("%s: + active threads=%ld\n",  prefix, this->pool_n_eltos) ;

    // Return OK
    return 1 ;
}


