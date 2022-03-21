
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MFS.
 *
 *  MFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MFS_PROTOCOL_H__
#define __MFS_PROTOCOL_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_comm.h"


    /*
     * Messages
     */

    // Connection
    #define REQ_ACTION_NONE         0
    #define REQ_ACTION_DISCONNECT   1
    #define REQ_ACTION_SHUTDOWN     2

    // Files
    #define REQ_ACTION_OPEN        10
    #define REQ_ACTION_CLOSE       11
    #define REQ_ACTION_READ        12
    #define REQ_ACTION_WRITE       13

    // Directories
    #define REQ_ACTION_MKDIR       20
    #define REQ_ACTION_RMDIR       21

    // DBM Files
    #define REQ_ACTION_DBMOPEN     30
    #define REQ_ACTION_DBMCLOSE    31
    #define REQ_ACTION_DBMSTORE    32
    #define REQ_ACTION_DBMFETCH    33
    #define REQ_ACTION_DBMDELETE   34


    // Datatypes
    typedef struct
    {
        long  req_action ;
        long  req_arg1 ;
        long  req_arg2 ;
    } msg_t ;

    typedef struct
    {
	void        *buff ;
	int          size ;
	MPI_Datatype datatype ;
	int          remote ;
	int          comm_action ;
	char        *err_msg ;
    } buffer_t ;


    // Communications
    int mfs_protocol_request_send    ( comm_t *cb, int rank, msg_t *msg ) ;
    int mfs_protocol_request_receive ( comm_t *cb,           msg_t *msg ) ;

#endif

