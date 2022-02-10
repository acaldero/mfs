
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

#include "mfs_pool.h"


th_args_t  buffer[MAX_BUFFER];
    pthread_t ths[MAX_SERVICIO];

int  n_elementos  = 0;
int  ha_arrancado = 0;
int  fin = 0;
int  pos_receptor = 0;

pthread_mutex_t  mutex;
pthread_cond_t   no_lleno;
pthread_cond_t   no_vacio;
pthread_cond_t   arrancado;
pthread_cond_t   parado;


void * th_pool_service ( void * param )
{
      th_args_t p;
      int pos_servicio = 0;

      // signal initializate...
      pthread_mutex_lock(&mutex);
      ha_arrancado = 1;
      pthread_cond_signal(&arrancado);
      pthread_mutex_unlock(&mutex);

      for (;;)
      {
	   // lock when not empty and not ended...
           pthread_mutex_lock(&mutex);
           while (n_elementos == 0)
	   {
                if (fin==1) {
                     fprintf(stderr,"th_pool_service: finalizando\n");
                     pthread_cond_signal(&parado);
                     pthread_mutex_unlock(&mutex);
                     pthread_exit(0);
                }

                pthread_cond_wait(&no_vacio, &mutex);
           } // while

	   // removing element from buffer...
           memmove(&p, &(buffer[pos_servicio]), sizeof(th_args_t)) ;
           pos_servicio = (pos_servicio + 1) % MAX_BUFFER;
           n_elementos--;

	   // signal not full...
           pthread_cond_signal(&no_lleno);
           pthread_mutex_unlock(&mutex);

	   // process and response...
           fprintf(stderr, "th_pool_service: sirviendo posicion %d\n", pos_servicio);
	   p.function(p) ;
    }

    pthread_exit(0);
    return NULL;
}

int  mfs_pool_launch    ( comm_t *wb, void (*worker_function)(th_args_t) )
{

    // lock when not full...
    pthread_mutex_lock(&mutex);
    while (n_elementos == MAX_BUFFER) {
           pthread_cond_wait(&no_lleno, &mutex);
    }

    // inserting element into the buffer
    buffer[pos_receptor].ab       = *wb ;
    buffer[pos_receptor].function = worker_function ;
    pos_receptor = (pos_receptor +1) % MAX_BUFFER;
    n_elementos++;

    // signal not empty...
    pthread_cond_signal(&no_vacio);
    pthread_mutex_unlock(&mutex);

    // return OK
    return 1;
}

int  mfs_pool_init ( void )
{
    // initialize
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&no_lleno, NULL);
    pthread_cond_init(&no_vacio, NULL);
    pthread_cond_init(&arrancado, NULL);
    pthread_cond_init(&parado, NULL);

    // create threads
    for (int i=0; i<MAX_SERVICIO; i++)
    {
          pthread_create(&ths[i], NULL, th_pool_service, NULL);

          // wait thread is started
          pthread_mutex_lock(&mutex) ;
	  while (!ha_arrancado) {
                 pthread_cond_wait(&arrancado, &mutex) ;
	  }
          ha_arrancado = 0 ;
          pthread_mutex_unlock(&mutex) ;
    }

    // return OK
    return 1;
}

int  mfs_pool_waitall   ( void )
{
    // finalizar
    fin = 1;
    pthread_cond_broadcast(&no_vacio);

    for (int i=0; i<MAX_SERVICIO; i++) {
         pthread_join(ths[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&no_lleno);
    pthread_cond_destroy(&no_vacio);
    pthread_cond_destroy(&arrancado);
    pthread_cond_destroy(&parado);

    // return OK
    return 1;
}

