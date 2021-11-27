
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


#include "mfs_client_stub.h"


int clientstub_init ( client_stub_t *wb, int *argc, char ***argv )
{
    int ret ;

    // Read server port...
    ret = mfs_read_server_port(wb->port_name) ;
    if (ret < 0) {
        fprintf(stderr, "ERROR: server port name required in the 'mfs_server.port' file.\n");
        return -1 ;
    }

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

    // Connect...
    ret = MPI_Comm_connect(wb->port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_connect fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}

int clientstub_request ( client_stub_t *wb, int req_action, int req_id )
{
    int ret ;
    int buff[3] ;

    buff[0] = req_action ;
    buff[1] = req_id ;
    buff[2] = 0 ;
    ret = MPI_Send(buff, 3, MPI_INT, 0, 2, wb->server) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int clientstub_finalize ( client_stub_t *wb )
{
    int ret ;

    // Remote disconnect...
    clientstub_request(wb, REQ_ACTION_DISCONNECT, 0) ;

    // Disconnect...
    ret = MPI_Comm_disconnect(&(wb->server)) ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Comm_disconnect fails :-S") ;
        return -1 ;
    }

    // Finalize...
    ret = MPI_Finalize() ;
    if (MPI_SUCCESS != ret) {
        fprintf(stderr, "ERROR: MPI_Finalize fails :-S") ;
        return -1 ;
    }

    // Return OK
    return 0 ;
}


/*
 *  File System API
 */

int clientstub_open ( client_stub_t *wb, const char *pathname, int flags )
{
    int ret ;
    int buff[3] ;

    // Send open msg
    buff[0] = REQ_ACTION_OPEN ;
    buff[1] = strlen(pathname) ;
    buff[2] = flags ;

        ret = MPI_Send(buff, 3, MPI_INT, 0, 2, wb->server) ;
    if (MPI_SUCCESS == ret)
	ret = MPI_Send(pathname, buff[1], MPI_CHAR, 0, 2, wb->server) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int clientstub_close ( client_stub_t *wb, int fd )
{
    int ret ;
    int buff_int[3] ;

    // Send close msg
    buff_int[0] = REQ_ACTION_CLOSE ;
    buff_int[1] = fd ;
    buff_int[2] = 0 ;

    ret = MPI_Send(buff_int, 3, MPI_INT, 0, 2, wb->server) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int clientstub_read ( client_stub_t *wb, int fd, void *buff_char, int count )
{
    int ret ;
    int buff_int[3] ;
    MPI_Status status ;

    // Send open msg
    buff_int[0] = REQ_ACTION_READ ;
    buff_int[1] = fd ;
    buff_int[2] = count ;

        ret = MPI_Send(buff_int,      3,  MPI_INT, 0, 2, wb->server) ;
    if (MPI_SUCCESS == ret)
	ret = MPI_Recv(buff_char, count, MPI_CHAR, 0, 2, wb->server, &status) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

int clientstub_write ( client_stub_t *wb, int fd, void *buff_char, int count )
{
    int ret ;
    int buff_int[3] ;

    // Send open msg
    buff_int[0] = REQ_ACTION_WRITE ;
    buff_int[1] = fd ;
    buff_int[2] = count ;

        ret = MPI_Send(buff_int,     3,   MPI_INT, 0, 2, wb->server) ;
    if (MPI_SUCCESS == ret)
	ret = MPI_Send(buff_char, count, MPI_CHAR, 0, 2, wb->server) ;

    // Return OK/KO
    return (MPI_SUCCESS == ret) ;
}

