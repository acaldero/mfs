
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

#ifndef __CLIENT_STUB_SOCKET_H__
#define __CLIENT_STUB_SOCKET_H__

    // Includes
    #include "base_lib.h"
    #include "info_fsconf.h"
    #include "mfs_comm.h"
    #include "mfs_comm_socket.h"
    #include "stub_msg.h"


    // File System API
    int clientstub_socket_init            ( comm_t *wb, params_t *params, int g_size, int g_rank ) ;
    int clientstub_socket_finalize        ( comm_t *wb, params_t *params ) ;

    int clientstub_socket_open_partition_element  ( comm_t *wb, params_t *params, conf_part_t *partition, int remote_rank ) ;
    int clientstub_socket_close_partition_element ( comm_t *wb, params_t *params, conf_part_t *partition ) ;

    // File API
    long clientstub_socket_open    ( comm_t *wb, const char *pathname, int flags ) ;
    int  clientstub_socket_close   ( comm_t *wb, long fd ) ;
    int  clientstub_socket_read    ( comm_t *wb, long fd, void *buf, int count ) ;
    int  clientstub_socket_write   ( comm_t *wb, long fd, void *buf, int count ) ;

    // Directory API
    long clientstub_socket_mkdir   ( comm_t *wb, const char *pathname, int mode ) ;
    long clientstub_socket_rmdir   ( comm_t *wb, const char *pathname ) ;

    // DBM File API
    long clientstub_socket_dbmopen   ( comm_t *wb, const char *pathname, int flags ) ;
    int  clientstub_socket_dbmclose  ( comm_t *wb, long fd ) ;
    int  clientstub_socket_dbmstore  ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int  count_val ) ;
    int  clientstub_socket_dbmfetch  ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val ) ;
    int  clientstub_socket_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key ) ;

#endif

