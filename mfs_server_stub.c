
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

//
// Thread counter API
//

void th_inc ( server_stub_t *wb )
{
    wb->at_m.lock() ;
    fprintf(stdout, "INFO: active_threads++\n");
    wb->active_threads++ ;
    wb->at_m.unlock() ;
}

void th_dec ( server_stub_t *wb )
{
    wb->at_m.lock() ;
    fprintf(stdout, "INFO: active_threads--\n");
    wb->active_threads-- ;
    if (0 == wb->active_threads) {
	wb->at_c.notify_all() ;
    }
    wb->at_m.unlock() ;
}

void th_wait ( server_stub_t *wb )
{
    std::unique_lock<std::mutex> lk(wb->at_m);

    lk.lock() ;
    while (wb->active_threads != 0) {
           wb->at_c.wait(lk) ;
    }
    lk.unlock() ;
}

//
// Server stub
//

int serverstub_init ( server_stub_t *wb, int *argc, char ***argv )
{
    int ret ;

    // MPI_Init
    ret = MPI_Init(argc, argv);
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Init fails :-S");
        return -1 ;
    }

    // wb->size = comm_size()
    ret = MPI_Comm_size(MPI_COMM_WORLD, &(wb->size));
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_size fails :-S");
        return -1 ;
    }

    // wb->rank = comm_rank()
    ret = MPI_Comm_rank(MPI_COMM_WORLD, &(wb->rank));
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_rank fails :-S");
        return -1 ;
    }

    // Open server port...
    ret = MPI_Open_port(MPI_INFO_NULL, wb->port_name);
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Open_port fails :-S");
        return -1 ;
    }

    // Write server port...
    ret = mfs_write_server_port(wb->port_name) ;
    if (ret < 0) {
        return -1 ;
    }

    // wb->the_end/active_threads
    wb->the_end        = 0 ; // false
    wb->active_threads = 0 ;

    // Return OK
    return 0 ;
}

int serverstub_finalize ( server_stub_t *wb )
{
    int ret ;

    // Wait for active requests...
    fprintf(stdout, "INFO: Server[%d] wait for threads...\n", wb->rank) ;
    th_wait(wb) ;

    // Close port
    MPI_Close_port(wb->port_name) ;

    // Finalize
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Finalize fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int serverstub_accept ( server_stub_t *wb, 
		        int (*do_srv)(MPI_Comm *, server_stub_t *) )
{
    MPI_Comm client ;

    fprintf(stdout, "INFO: Server[%d] accepting...\n", wb->rank);
    MPI_Comm_accept(wb->port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &client);

    fprintf(stdout, "INFO: Server[%d] create new thread...\n", wb->rank);
    th_inc(wb) ;
    std::thread t1(do_srv, &client, wb) ;
    if (t1.joinable()) {
        t1.detach() ;
    }
}

int serverstub_request_recv ( MPI_Comm client,
		           // server_stub_t *wb, 
		              int *req_action, int *req_id )
{
    int ret ;
    int buff[2] ;
    MPI_Status status;

    ret = MPI_Recv(buff, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, client, &status) ;
    *req_action = buff[0] ;
    *req_id     = buff[1] ;

    return (MPI_SUCCESS == ret) ;
}

int serverstub_request_send ( MPI_Comm server,
                           // server_stub_t *wb, 
			      int req_action, int req_id )
{
    int ret ;
    int buff[2] ;

    buff[0] = req_action ;
    buff[1] = req_id ;
    ret = MPI_Send(buff, 2, MPI_INT, 0, 2, server) ;

    // Return OK
    return (MPI_SUCCESS == ret) ;
}


