
/*
 *  Copyright 2020-2021 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


#include <mfs_server_stub.h>


// to wait for copy arguments and for active threads...
int sync_copied    = 0 ;
int active_threads = 0 ;
pthread_mutex_t sync_mutex ;
pthread_cond_t  sync_cond ;
pthread_cond_t   end_cond ;

pthread_attr_t attr ;
pthread_t thid ;


void *do_srv ( void *wb )
{
    int again;
    int req_action ;
    int req_id ;
    server_stub_t ab ;

    // copy arguments and signal...
    pthread_mutex_lock(&sync_mutex) ;
    ab = *((server_stub_t *)wb) ;
    sync_copied = 1 ;
    active_threads++ ;
    pthread_cond_signal(&sync_cond) ;
    pthread_mutex_unlock(&sync_mutex) ;

    // request loop...
    again = 1;
    while (again)
    {
          printf("INFO: Server[%d] receiving...\n", ab.rank);
          serverstub_request_recv(&ab, &req_action, &req_id) ;
          switch (req_action)
          {
              case REQ_ACTION_END:
	           printf("INFO: END\n") ;
	           MPI_Comm_free(&(ab.client)) ;
	           return 0;

              case REQ_ACTION_DISCONNECT:
	           printf("INFO: Disconnect\n") ;
	           MPI_Comm_disconnect(&(ab.client)) ;
	           again = 0 ;
	           break;

              case REQ_ACTION_DATA:
	           printf("INFO: Received: %d\n", req_id) ;
	           break;

              default:
	           printf("ERROR: Unexpected message type: %d\n", req_action) ;
          }
    }

    // active_thread--...
    fprintf(stdout, "INFO: active_threads--\n");
    pthread_mutex_lock(&sync_mutex) ;
    active_threads-- ;
    if (0 == active_threads) {
        pthread_cond_signal(&end_cond) ;
    }
    pthread_mutex_unlock(&sync_mutex) ;

    return NULL;
}


/*
 *  Main
 */

int main ( int argc, char **argv )
{
    int ret ;
    server_stub_t wb ;
    server_stub_t ab ;

    // Welcome...
    fprintf(stdout, "\n"
 		    " mfs_server (0.5)\n"
		    " ----------------\n"
		    "\n");

    // Initialize...
    fprintf(stdout, "INFO: Server initializing...\n") ;
    ret = serverstub_init(&wb, &argc, &argv) ;
    if (ret < 0) {
        fprintf(stderr, "ERROR: serverstub_init fails :-S") ;
        return -1 ;
    }

    active_threads = 0 ;
    sync_copied    = 0 ;
    pthread_mutex_init(&sync_mutex, NULL) ;
    pthread_cond_init(&sync_cond, NULL) ;
    pthread_cond_init(&end_cond, NULL) ;
    pthread_attr_init(&attr) ;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) ;

    // To serve requests...
    fprintf(stdout, "INFO: Server[%d] accepting...\n", wb.rank) ;
    serverstub_accept(&ab, &wb) ;
    while (serverstub_get_theend(&ab))
    {
	// new thread...
	fprintf(stdout, "INFO: Server[%d] create new thread...\n", ab.rank) ;
	pthread_create(&thid, &attr, do_srv, (void *)&ab) ;

	// wait to copy arguments + active_thread++...
	pthread_mutex_lock(&sync_mutex) ;
        while (sync_copied == 0) {
               pthread_cond_wait(&sync_cond, &sync_mutex) ;
        }
        sync_copied = 0 ;
        pthread_mutex_unlock(&sync_mutex) ;

	// To serve next request...
        fprintf(stdout, "INFO: Server[%d] accepting...\n", wb.rank) ;
        serverstub_accept(&ab, &wb) ;
    }

    // Wait for active_thread...
    fprintf(stdout, "INFO: Server[%d] wait for threads...\n", wb.rank) ;
    pthread_mutex_lock(&sync_mutex) ;
    while (active_threads != 0) {
           pthread_cond_wait(&end_cond, &sync_mutex) ;
    }
    pthread_mutex_unlock(&sync_mutex) ;

    // Finalize...
    fprintf(stdout, "INFO: Server[%d] ends.\n", wb.rank) ;
    serverstub_finalize(&wb) ;

    return 0 ;
}

