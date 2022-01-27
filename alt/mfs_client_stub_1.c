
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
    int ret = 0 ;

    // Initialize
    if (ret >= 0)
    {
        ret = mfs_comm_init(wb, argc, argv) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: initialization fails :-(", -1) ;
        }
    }

    // Register service
    if (ret >= 0)
    {
        sprintf(wb->srv_name, "%s.%d", MFS_SERVER_STUB_PNAME, wb->rank) ;

        ret = mfs_comm_connect(wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: connection fails :-(", wb->rank) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int clientstub_finalize ( comm_t *wb )
{
    int ret = 0 ;

    // Remote disconnect...
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_DISCONNECT, 0, 0) ;
    }

    // Disconnect...
    if (ret >= 0)
    {
        ret = mfs_comm_disconnect(wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: disconnect fails :-(", wb->rank) ;
        }
    }

    // Finalize
    if (ret >= 0)
    {
        ret = mfs_comm_finalize(wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: finalization fails :-(", wb->rank) ;
        }
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File System API
 */

int clientstub_open ( comm_t *wb, const char *pathname, int flags )
{
    int ret = 0 ;
    int fd  = -1 ;

    // Send open msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_OPEN, strlen(pathname) + 1, flags) ;
    }

    // Send pathname
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(wb, 0, (void *)pathname, strlen(pathname) + 1, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: pathname cannot be sent :-(", wb->rank) ;
        }
    }

    // Receive descriptor
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, &fd, 1, MPI_INT) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: file descriptor not received :-(", wb->rank) ;
        }
    }

    // Return file descriptor
    return fd ;
}

int clientstub_close ( comm_t *wb, int fd )
{
    int ret = 0 ;

    // Send close msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_CLOSE, fd, 0) ;
    }

    // Return OK/KO
    return ret ;
}

int clientstub_read ( comm_t *wb, int fd, void *buff_char, int count )
{
    int ret = 0 ;

    // Send read msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_READ, fd, count) ;
    }

    // Receive data
    if (ret >= 0)
    {
        ret = mfs_comm_recv_data_from(wb, 0, buff_char, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: data not received :-(", wb->rank) ;
        }
    }

    // Return OK/KO
    return ret ;
}

int clientstub_write ( comm_t *wb, int fd, void *buff_char, int count )
{
    int ret = 0 ;

    // Send write msg
    if (ret >= 0)
    {
        ret = mfs_comm_request_send(wb, 0, REQ_ACTION_WRITE, fd, count) ;
    }

    // Send data
    if (ret >= 0)
    {
        ret = mfs_comm_send_data_to(wb, 0, buff_char, count, MPI_CHAR) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: data cannot be sent :-(", wb->rank) ;
        }
    }

    // Return OK/KO
    return ret ;
}

