
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


#include <signal.h>
#include <pthread.h>
#include "mfs_protocol.h"
#include "mfs_server_stub.h"


// to wait for copy arguments and for active threads...
int sync_copied    = 0 ;
int active_threads = 0 ;
int the_end        = 0 ;
pthread_mutex_t sync_mutex ;
pthread_cond_t  sync_cond ;
pthread_cond_t   end_cond ;

// stats
long   t1 = 0L ;
char *ver = "0.8" ;


void do_stats_ctrc ( int sigid )
{
    long t2 = mfs_get_time() ;

    printf("INFO:\n") ;
    printf("INFO: Threads:\n") ;
    printf("INFO: + active threads=%ld\n", active_threads) ;
    printf("INFO: Time:\n") ;
    printf("INFO: + running time=%lf seconds.\n", (t2-t1)/1000.0) ;
    printf("INFO: Version:\n") ;
    printf("INFO: + server=%s\n", ver) ;
    printf("INFO:\n") ;
}

void *do_srv ( void *wb )
{
    int    again ;
    comm_t ab ;
    msg_t  msg ;
    long   ret ;

    // copy arguments and signal...
    pthread_mutex_lock(&sync_mutex) ;
    ab = *((comm_t *)wb) ;
    sync_copied = 1 ;
    active_threads++ ;
    pthread_cond_signal(&sync_cond) ;
    pthread_mutex_unlock(&sync_mutex) ;
    mfs_print(DBG_INFO, "Server[%d]: active_threads++\n", ab.rank) ;

    // request loop...
    again = 1 ;
    while (again)
    {
          mfs_print(DBG_INFO, "Server[%d]: receiving...\n", ab.rank) ;
          mfs_protocol_request_receive(&ab, &msg) ;
          switch (msg.req_action)
          {
              case REQ_ACTION_NONE:
	           mfs_print(DBG_INFO, "Server[%d]: request 'none'\n", ab.rank) ;
	           break;

              case REQ_ACTION_DISCONNECT:
	           mfs_print(DBG_INFO, "Server[%d]: request 'disconnect'\n", ab.rank) ;
	           again = 0 ;
	           break;

	      case REQ_ACTION_OPEN:
	           mfs_print(DBG_INFO, "Server[%d]: request 'open' for a filename of %d chars\n", ab.rank, msg.req_action) ;
                   ret = serverstub_open(&ab, msg.req_arg1, msg.req_arg2) ;
                   mfs_print(DBG_INFO, "Server[%d]: File[%d]: open(flags=%d)\n", ab.rank, ret, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_CLOSE:
	           mfs_print(DBG_INFO, "Server[%d]: File[%d]: close()\n", ab.rank, msg.req_arg1) ;
                   ret = serverstub_close(&ab, msg.req_arg1) ;
	           break;

	      case REQ_ACTION_READ:
	           mfs_print(DBG_INFO, "Server[%d]: File[%d]: read(bytes=%d)\n", ab.rank, msg.req_arg1, msg.req_arg2) ;
                   ret = serverstub_read(&ab, msg.req_arg1, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_WRITE:
	           mfs_print(DBG_INFO, "Server[%d]: File[%d]: write(bytes=%d)\n", ab.rank, msg.req_arg1, msg.req_arg2) ;
                   ret = serverstub_write(&ab, msg.req_arg1, msg.req_arg2) ;
	           break;

              default:
	           mfs_print(DBG_ERROR, "Server[%d]: unexpected message type: %d\n", ab.rank, msg.req_action) ;
          }
    }

    // active_thread--...
    pthread_mutex_lock(&sync_mutex) ;
    active_threads-- ;
    if (0 == active_threads) {
        pthread_cond_signal(&end_cond) ;
    }
    pthread_mutex_unlock(&sync_mutex) ;
    mfs_print(DBG_INFO, "Server[%d]: active_threads--\n", ab.rank) ;

    // disconnect from server
    serverstub_disconnect(&ab) ;

    // return
    return NULL ;
}


/*
 *  Main
 */

int main ( int argc, char **argv )
{
    int ret ;
    comm_t wb ;
    comm_t ab ;
    pthread_attr_t attr ;
    pthread_t thid ;

    // Welcome...
    printf("\n"
 	   " mfs_server\n"
	   " ----------\n"
	   "\n") ;

    // Initialize stub...
    mfs_print(DBG_INFO, "Server[%d]: initializing...\n", -1) ;
    ret = serverstub_init(&wb, &argc, &argv) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: serverstub_init fails :-(", -1) ;
        return -1 ;
    }

    // Initialize global variables
    active_threads = 0 ;
    sync_copied    = 0 ;
    pthread_mutex_init(&sync_mutex, NULL) ;
    pthread_cond_init(&sync_cond, NULL) ;
    pthread_cond_init(&end_cond, NULL) ;
    pthread_attr_init(&attr) ;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) ;

    signal(SIGUSR1, do_stats_ctrc) ;
    t1 = mfs_get_time() ;

    // To serve requests...
    mfs_print(DBG_INFO, "Server[%d]: accepting...\n", wb.rank) ;
    ret = serverstub_accept(&ab, &wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: accept fails :-(", -1) ;
    }
    while (0 == the_end)
    {
	// new thread...
	mfs_print(DBG_INFO, "Server[%d]: create new thread...\n", ab.rank) ;
	pthread_create(&thid, &attr, do_srv, (void *)&ab) ;

	// wait to copy arguments + active_thread++...
	pthread_mutex_lock(&sync_mutex) ;
        while (sync_copied == 0) {
               pthread_cond_wait(&sync_cond, &sync_mutex) ;
        }
        sync_copied = 0 ;
        pthread_mutex_unlock(&sync_mutex) ;

	// To serve next request...
        mfs_print(DBG_INFO, "Server[%d]: accepting...\n", wb.rank) ;
        ret = serverstub_accept(&ab, &wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: accept fails :-(", -1) ;
        }
    }

    // Wait for active_thread...
    mfs_print(DBG_INFO, "Server[%d]: wait for threads...\n", wb.rank) ;
    pthread_mutex_lock(&sync_mutex) ;
    while (active_threads != 0) {
           pthread_cond_wait(&end_cond, &sync_mutex) ;
    }
    pthread_mutex_unlock(&sync_mutex) ;

    // Finalize...
    mfs_print(DBG_INFO, "Server[%d]: ends.\n", wb.rank) ;
    serverstub_finalize(&wb) ;

    return 0 ;
}

