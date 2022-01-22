
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

#ifndef __MFS_PROTOCOL_H__
#define __MFS_PROTOCOL_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_comm.h"


    // Messages
    #define REQ_ACTION_NONE       0
    #define REQ_ACTION_DISCONNECT 1

    #define REQ_ACTION_OPEN       10
    #define REQ_ACTION_CLOSE      11
    #define REQ_ACTION_READ       12
    #define REQ_ACTION_WRITE      13


    // Datatypes
    typedef struct
    {
        int  req_action ;
        int  req_arg1 ;
        int  req_arg2 ;
    } msg_t ;


    // Communications
    int mfs_protocol_request_send    ( comm_t *cb, int rank, msg_t *msg ) ;
    int mfs_protocol_request_receive ( comm_t *cb,           msg_t *msg ) ;

#endif

