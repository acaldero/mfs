
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


#include "mfs_server_stub.h"
#include "mfs_files.h"


// to wait for copy arguments and for active threads...
int sync_copied    = 0 ;
int active_threads = 0 ;
int the_end        = 0 ;
pthread_mutex_t sync_mutex ;
pthread_cond_t  sync_cond ;
pthread_cond_t   end_cond ;


void *do_srv ( void *wb )
{
    int again;
    server_stub_t ab ;
    int buff_int[3] ;
    int ret ;
    char pathname[1024] ;
    char *buff_data ;
    int fd ;

    // copy arguments and signal...
    mfs_print(stdout, "INFO: active_threads++\n") ;
    pthread_mutex_lock(&sync_mutex) ;
    ab = *((server_stub_t *)wb) ;
    sync_copied = 1 ;
    active_threads++ ;
    pthread_cond_signal(&sync_cond) ;
    pthread_mutex_unlock(&sync_mutex) ;

    // request loop...
    again = 1 ;
    while (again)
    {
          mfs_print(stdout, "INFO: Server[%d] receiving...\n", ab.rank) ;
          serverstub_request_recv(&ab, buff_int, 3, MPI_INT) ;
          switch (buff_int[0])
          {
              case REQ_ACTION_NONE:
	           mfs_print(stdout, "INFO: None\n") ;
	           break;

              case REQ_ACTION_DISCONNECT:
	           mfs_print(stdout, "INFO: Disconnect\n") ;
	           again = 0 ;
	           break;

	      case REQ_ACTION_OPEN:
	           mfs_print(stdout, "INFO: Open a filename of %d chars\n", buff_int[1]) ;
                   ret = serverstub_request_recv(&ab, pathname, buff_int[1], MPI_CHAR) ;
	           mfs_print(stdout, "INFO: Open('%s', %d)\n", pathname, buff_int[2]) ;
                   fd = server_files_open(pathname, buff_int[2]) ;
		   ret = serverstub_request_send(&ab, &fd, 1, MPI_INT) ;
	           break;

	      case REQ_ACTION_CLOSE:
	           mfs_print(stdout, "INFO: Close(%d);\n", buff_int[1]) ;
                   server_files_close(fd) ;
	           break;

	      case REQ_ACTION_READ:
	           mfs_print(stdout, "INFO: Read(%d, data, %d)\n", buff_int[1], buff_int[2]) ;
		   buff_data = (char *)malloc(buff_int[2]) ;
		   memset(buff_data, 0, buff_int[2]) ;
                   server_files_read(buff_int[1], buff_data, buff_int[2]) ;
		   ret = serverstub_request_send(&ab, buff_data, buff_int[2], MPI_CHAR) ;
		   free(buff_data) ;
	           break;

	      case REQ_ACTION_WRITE:
	           mfs_print(stdout, "INFO: Write(%d, data, %d)\n", buff_int[1], buff_int[2]) ;
		   buff_data = (char *)malloc(buff_int[2]) ;
		   memset(buff_data, 0, buff_int[2]) ;
                   ret = serverstub_request_recv(&ab, buff_data, buff_int[2], MPI_CHAR) ;
                   server_files_write(buff_int[1], buff_data, buff_int[2]) ;
		   free(buff_data) ;
	           break;

              default:
	           mfs_print(stdout, "ERROR: Unexpected message type: %d\n", buff_int[0]) ;
          }
    }

    // active_thread--...
    mfs_print(stdout, "INFO: active_threads--\n") ;
    pthread_mutex_lock(&sync_mutex) ;
    active_threads-- ;
    if (0 == active_threads) {
        pthread_cond_signal(&end_cond) ;
    }
    pthread_mutex_unlock(&sync_mutex) ;

    // serverstub_request_disconnect(...)
    MPI_Comm_disconnect(&(ab.client)) ;

    // return
    return NULL ;
}


/*
 *  Main
 */

int main ( int argc, char **argv )
{
    int ret ;
    server_stub_t wb ;
    server_stub_t ab ;
    pthread_attr_t attr ;
    pthread_t thid ;

    // Welcome...
    fprintf(stdout, "\n"
 		    " mfs_server (0.7)\n"
		    " ----------------\n"
		    "\n");

    // Initialize...
    mfs_print(stdout, "INFO: Server initializing...\n") ;
    ret = serverstub_init(&wb, &argc, &argv) ;
    if (ret < 0) {
        mfs_print(stderr, "ERROR: serverstub_init fails :-S") ;
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
    mfs_print(stdout, "INFO: Server[%d] accepting...\n", wb.rank) ;
    serverstub_accept(&ab, &wb) ;
    while (0 == the_end)
    {
	// new thread...
	mfs_print(stdout, "INFO: Server[%d] create new thread...\n", ab.rank) ;
	pthread_create(&thid, &attr, do_srv, (void *)&ab) ;

	// wait to copy arguments + active_thread++...
	pthread_mutex_lock(&sync_mutex) ;
        while (sync_copied == 0) {
               pthread_cond_wait(&sync_cond, &sync_mutex) ;
        }
        sync_copied = 0 ;
        pthread_mutex_unlock(&sync_mutex) ;

	// To serve next request...
        mfs_print(stdout, "INFO: Server[%d] accepting...\n", wb.rank) ;
        serverstub_accept(&ab, &wb) ;
    }

    // Wait for active_thread...
    mfs_print(stdout, "INFO: Server[%d] wait for threads...\n", wb.rank) ;
    pthread_mutex_lock(&sync_mutex) ;
    while (active_threads != 0) {
           pthread_cond_wait(&end_cond, &sync_mutex) ;
    }
    pthread_mutex_unlock(&sync_mutex) ;

    // Finalize...
    mfs_print(stdout, "INFO: Server[%d] ends.\n", wb.rank) ;
    serverstub_finalize(&wb) ;

    return 0 ;
}

