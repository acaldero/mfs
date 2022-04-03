
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

#ifndef __STUB_MSG_H__
#define __STUB_MSG_H__

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
        long  req_arg3 ;
    } msg_t ;

#endif

