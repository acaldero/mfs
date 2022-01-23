
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


#include "mfs_client_stub.h"


int clientstub_init ( comm_t *wb, int *argc, char ***argv )
{
    int ret ;

    // Initialize
    ret = mfs_comm_init(wb, argc, argv) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: initialization fails :-(", -1) ;
        return -1 ;
    }

    // Register service
    sprintf(wb->srv_name, "%s.%d", MFS_SERVER_STUB_PNAME, wb->rank) ;

    ret = mfs_comm_connect(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: connection fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int clientstub_finalize ( comm_t *wb )
{
    int ret ;
    msg_t msg ;

    // Remote disconnect...
    msg.req_action = REQ_ACTION_DISCONNECT ;
    msg.req_arg1   = 0 ;
    msg.req_arg2   = 0 ;

    ret = mfs_protocol_request_send(wb, 0, &msg) ;
    if (ret < 0) {
        return -1 ;
    }

    // Disconnect...
    ret = mfs_comm_disconnect(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: disconnect fails :-(", wb->rank) ;
        return -1 ;
    }

    // Finalize
    ret = mfs_comm_finalize(wb) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Server[%d]: finalization fails :-(", wb->rank) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int clientstub_open ( comm_t *wb, const char *pathname, int flags )
{
    int      ret, fd ;
    msg_t    msg ;
    buffer_t info[3] ;

    // Send open request
    msg.req_action = REQ_ACTION_OPEN ;
    msg.req_arg1   = strlen(pathname) + 1 ;
    msg.req_arg2   = flags ;

    info[0].buff        = (void *)&msg ;
    info[0].size        = 3 ;
    info[0].datatype    = MPI_INT ;
    info[0].remote      = 0 ;
    info[0].err_msg     = "Client[%d]: open request cannot be sent :-(" ;
    info[0].comm_action = COM_SEND_DATA_TO ;

    // Send pathname
    info[1].buff        = (void *)pathname ;
    info[1].size        = strlen(pathname) + 1 ;
    info[1].datatype    = MPI_CHAR ;
    info[1].remote      = 0 ;
    info[1].err_msg     = "Client[%d]: pathname cannot be sent :-(" ;
    info[1].comm_action = COM_SEND_DATA_TO ;

    // Receive descriptor
    info[2].buff        = (void *)&fd ;
    info[2].size        = 1 ;
    info[2].datatype    = MPI_INT ;
    info[2].remote      = 0 ;
    info[2].err_msg     = "Client[%d]: file descriptor not received :-(" ;
    info[2].comm_action = COM_RECV_DATA_FROM ;

    ret = mfs_protocol_request_do(wb, info, 3) ;
    if (ret < 0) {
        return -1 ;
    }

    // Return file descriptor
    return fd ;
}

int clientstub_close ( comm_t *wb, int fd )
{
    int      ret ;
    msg_t    msg ;
    buffer_t info[1] ;

    // Send close request
    msg.req_action = REQ_ACTION_CLOSE ;
    msg.req_arg1   = fd ;
    msg.req_arg2   = 0 ;

    info[0].buff        = (void *)&msg ;
    info[0].size        = 3 ;
    info[0].datatype    = MPI_INT ;
    info[0].remote      = 0 ;
    info[0].err_msg     = "Client[%d]: close request cannot be sent :-(" ;
    info[0].comm_action = COM_SEND_DATA_TO ;

    ret = mfs_protocol_request_do(wb, info, 1) ;
    if (ret < 0) {
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int clientstub_read ( comm_t *wb, int fd, void *buff_char, int count )
{
    int      ret ;
    msg_t    msg ;
    buffer_t info[2] ;

    // Send read msg
    msg.req_action = REQ_ACTION_READ ;
    msg.req_arg1   = fd ;
    msg.req_arg2   = count ;

    info[0].buff        = (void *)&msg ;
    info[0].size        = 3 ;
    info[0].datatype    = MPI_INT ;
    info[0].remote      = 0 ;
    info[0].err_msg     = "Client[%d]: read request cannot be sent :-(" ;
    info[0].comm_action = COM_SEND_DATA_TO ;

    // Receive data
    info[1].buff        = buff_char ;
    info[1].size        = count ;
    info[1].datatype    = MPI_CHAR ;
    info[1].remote      = 0 ;
    info[1].err_msg     = "Client[%d]: data not received :-(" ;
    info[1].comm_action = COM_RECV_DATA_FROM ;

    ret = mfs_protocol_request_do(wb, info, 2) ;
    if (ret < 0) {
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int clientstub_write ( comm_t *wb, int fd, void *buff_char, int count )
{
    int      ret ;
    msg_t    msg ;
    buffer_t info[2] ;

    // Send write msg
    msg.req_action = REQ_ACTION_WRITE ;
    msg.req_arg1   = fd ;
    msg.req_arg2   = count ;

    info[0].buff        = (void *)&msg ;
    info[0].size        = 3 ;
    info[0].datatype    = MPI_INT ;
    info[0].remote      = 0 ;
    info[0].err_msg     = "Client[%d]: write request cannot be sent :-(" ;
    info[0].comm_action = COM_SEND_DATA_TO ;

    // Send data
    info[1].buff        = buff_char ;
    info[1].size        = count ;
    info[1].datatype    = MPI_CHAR ;
    info[1].remote      = 0 ;
    info[1].err_msg     = "Client[%d]: data not sent :-(" ;
    info[1].comm_action = COM_SEND_DATA_TO ;

    ret = mfs_protocol_request_do(wb, info, 2) ;
    if (ret < 0) {
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

