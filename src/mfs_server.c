
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
   #include "mfs_params.h"
   #include "mfs_protocol.h"
   #include "mfs_server_stub.h"
   #include "mfs_workers.h"


   // stats
   char    *ver     = "0.9" ;
   int      the_end = 0 ;
   long     t1      = 0L ;
   params_t params ;

   void do_stats_ctrc ( int sigid )
   {
       long t2 = mfs_get_time() ;

       printf("INFO:\n") ;
       printf("INFO: Version:\n") ;
       printf("INFO: + server=%s\n", ver) ;
       printf("INFO: Time:\n") ;
       printf("INFO: + running time=%lf seconds.\n", (t2-t1)/1000.0) ;
       mfs_workers_stats_show("INFO") ;
       printf("INFO:\n") ;
   }

   void do_srv ( struct st_th th )
   {
       long      ret ;
       int       again ;
       file_t    fd ;
       msg_t     msg ;

       // request loop...
       again = 1 ;
       while (again)
       {
          mfs_print(DBG_INFO, "Server[%d]: waiting for request...\n", th.ab.rank) ;
          mfs_protocol_request_receive(&th.ab, &msg) ;
          switch (msg.req_action)
          {
              case REQ_ACTION_SHUTDOWN:
	           mfs_print(DBG_INFO, "Server[%d]: request 'shutdown'\n", th.ab.rank) ;
                   the_end = 1 ;
	           break;

              case REQ_ACTION_NONE:
	           mfs_print(DBG_INFO, "Server[%d]: request 'none'\n", th.ab.rank) ;
	           break;

              case REQ_ACTION_DISCONNECT:
              case REQ_ACTION_ATEXIT:
	           mfs_print(DBG_INFO, "Server[%d]: request 'disconnect'\n", th.ab.rank) ;
	           again = 0 ;
	           break;

              // Files
	      case REQ_ACTION_OPEN:
		   ret = mfs_file_long2fd(&fd, -1, params.file_protocol) ;
                   ret = serverstub_open(&th.ab, &fd, params.data_prefix, msg.req_arg1, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_CLOSE:
		   ret = mfs_file_long2fd(&fd, msg.req_arg1, params.file_protocol) ;
                   ret = serverstub_close(&th.ab, &fd) ;
	           break;

	      case REQ_ACTION_READ:
		   ret = mfs_file_long2fd(&fd, msg.req_arg1, params.file_protocol) ;
                   ret = serverstub_read(&th.ab, &fd, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_WRITE:
		   ret = mfs_file_long2fd(&fd, msg.req_arg1, params.file_protocol) ;
                   ret = serverstub_write(&th.ab, &fd, msg.req_arg2) ;
	           break;

              // Directories
	      case REQ_ACTION_MKDIR:
                   ret = serverstub_mkdir(&th.ab, params.data_prefix, msg.req_arg1, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_RMDIR:
                   ret = serverstub_rmdir(&th.ab, params.data_prefix, msg.req_arg1) ;
	           break;

              default:
	           mfs_print(DBG_ERROR, "Server[%d]: unexpected message type: %d\n", th.ab.rank, msg.req_action) ;
          }
       }

       // disconnect from server
       serverstub_disconnect(&(th.ab)) ;
   }


/*
 *  Main
 */

int main ( int argc, char **argv )
{
    int      ret ;
    comm_t   wb ;
    comm_t   ab ;

    // Welcome...
    printf("\n"
 	   " mfs_server\n"
	   " ----------\n"
	   "\n") ;

    // Get parameters..
    ret = mfs_params_get(&params, &argc, &argv) ;
    if (ret < 0) {
        mfs_params_show_usage() ;
        exit(-1) ;
    }

    mfs_print(DBG_INFO, "Server[%d]: initializing...\n", -1) ;
    mfs_params_show(&params) ;

    // Initialize workers
    ret = mfs_workers_init() ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: mfs_workers_init fails :-(", -1) ;
        return -1 ;
    }

    // Initialize stub...
    ret = serverstub_init(&wb, &params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: serverstub_init fails :-(", -1) ;
        return -1 ;
    }

    // Initialize signal + time
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
        ret = mfs_workers_launch_worker(&ab, do_srv) ;

	// To serve next request...
        mfs_print(DBG_INFO, "Server[%d]: accepting...\n", wb.rank) ;
        ret = serverstub_accept(&ab, &wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: accept fails :-(", -1) ;
        }
    }

    // Wait for active_thread...
    mfs_print(DBG_INFO, "Server[%d]: wait for threads...\n", wb.rank) ;
    mfs_workers_wait_workers() ;

    // Finalize...
    mfs_print(DBG_INFO, "Server[%d]: ends.\n", wb.rank) ;
    serverstub_finalize(&wb) ;

    return 0 ;
}
