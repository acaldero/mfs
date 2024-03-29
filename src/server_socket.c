
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


   #include <signal.h>
   #include "info_params.h"
   #include "stub_msg.h"
   #include "mfs_workers.h"
   #include "server_stub_socket.h"


   // stats
   char    *ver     = "1.00-socket" ;
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
       int       fd ;
       msg_t     msg ;

       // request loop...
       again = 1 ;
       while (again)
       {
          mfs_print(DBG_INFO, "Server[%d]: waiting for request...\n", th.ab.rank) ;
          ret = mfs_comm_socket_receive_request(&th.ab, MPI_ANY_SOURCE, &msg) ;
	  if (ret < 0)
	  {
	      mfs_print(DBG_ERROR, "Server[%d]: stub_msg_socket_request_receive with no more request\n", th.ab.rank) ;
	      again = 0 ;
	      continue ;
	  }

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
	           mfs_print(DBG_INFO, "Server[%d]: request 'disconnect'\n", th.ab.rank) ;
	           again = 0 ;
	           break;

              // Files
	      case REQ_ACTION_OPEN:
                   ret = serverstub_socket_open(&th.ab, &params, &fd, msg.req_arg1, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_CLOSE:
		   ret = mfs_file_long2fd(&(fd), msg.req_arg1) ;
                   ret = serverstub_socket_close(&th.ab, &params, fd) ;
	           break;

	      case REQ_ACTION_READ:
		   ret = mfs_file_long2fd(&(fd), msg.req_arg1) ;
                   ret = serverstub_socket_read(&th.ab, &params, fd, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_WRITE:
		   ret = mfs_file_long2fd(&(fd), msg.req_arg1) ;
                   ret = serverstub_socket_write(&th.ab, &params, fd, msg.req_arg2) ;
	           break;

              // Directories
	      case REQ_ACTION_MKDIR:
                   ret = serverstub_socket_mkdir(&th.ab, &params, msg.req_arg1, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_RMDIR:
                   ret = serverstub_socket_rmdir(&th.ab, &params, msg.req_arg1) ;
	           break;

              // DBM Files
	      case REQ_ACTION_DBMOPEN:
                   ret = serverstub_socket_dbmopen(&th.ab, &params, &fd, msg.req_arg1, msg.req_arg2) ;
	           break;

	      case REQ_ACTION_DBMCLOSE:
		   ret = mfs_file_long2fd(&(fd), msg.req_arg1) ;
                   ret = serverstub_socket_dbmclose(&th.ab, &params, fd) ;
	           break;

	      case REQ_ACTION_DBMSTORE:
		   ret = mfs_file_long2fd(&(fd), msg.req_arg1) ;
                   ret = serverstub_socket_dbmstore(&th.ab, &params, fd, msg.req_arg2, msg.req_arg3) ;
	           break;

	      case REQ_ACTION_DBMFETCH:
		   ret = mfs_file_long2fd(&(fd), msg.req_arg1) ;
                   ret = serverstub_socket_dbmfetch(&th.ab, &params, fd, msg.req_arg2, msg.req_arg3) ;
	           break;

	      case REQ_ACTION_DBMDELETE:
		   ret = mfs_file_long2fd(&(fd), msg.req_arg1) ;
                   ret = serverstub_socket_dbmdelete(&th.ab, &params, fd, msg.req_arg2) ;
	           break;

	      // Default
              default:
	           mfs_print(DBG_ERROR, "Server[%d]: unexpected message type: %d\n", th.ab.rank, msg.req_action) ;
          }
       }

       // disconnect from server
       serverstub_socket_disconnect_all(&(th.ab), &params) ;
   }


/*
 *  Main
 */

int main ( int argc, char **argv )
{
    int      ret, size, rank ;
    comm_t   wb ;
    comm_t   ab ;

    // Welcome...
    printf("\n"
 	   " mfs_server (sockets)\n"
	   " --------------------\n") ;

    // Get parameters..
    ret = info_params_get(&params, &argc, &argv) ;
    if (ret < 0) {
        info_params_show_usage() ;
        exit(-1) ;
    }

    if (params.verbose > 0) {
        info_params_show(&params) ;
        mfs_print(DBG_INFO, "Server[%d]: initializing...\n", -1) ;
    }

    // Get my rank
    MPI_Init(NULL, NULL) ;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank) ;
    MPI_Comm_size(MPI_COMM_WORLD, &size) ;
    MPI_Finalize() ;
    sleep(rank) ;

    // Initialize workers
    ret = mfs_workers_init(&params) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: mfs_workers_init fails :-(\n", -1) ;
        return -1 ;
    }

    // Initialize stub...
    ret = serverstub_socket_init(&wb, &params, size, rank) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: serverstub_socket_init fails :-(\n", -1) ;
        return -1 ;
    }

    // Initialize signal + time
    signal(SIGUSR1, do_stats_ctrc) ;
    t1 = mfs_get_time() ;

    // To serve requests...
    while (0 == the_end)
    {
	// To serve next request...
        mfs_print(DBG_INFO, "Server[%d]: accepting...\n", wb.rank) ;
        ret = serverstub_socket_accept(&ab, &params, &wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Server[%d]: accept fails :-(\n", -1) ;
        }

	// new thread...
	mfs_print(DBG_INFO, "Server[%d]: create new thread...\n", ab.rank) ;
        ret = mfs_workers_launch_worker(params.thread_launch, &ab, do_srv) ;
    }

    // Wait for active_thread...
    mfs_print(DBG_INFO, "Server[%d]: wait for threads...\n", wb.rank) ;
    mfs_workers_destroy() ;

    // Finalize...
    mfs_print(DBG_INFO, "Server[%d]: ends.\n", wb.rank) ;
    serverstub_socket_finalize(&wb, &params) ;

    return 0 ;
}

