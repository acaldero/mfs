
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

#ifndef __MFS_API_H__
#define __MFS_API_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_dbm.h"
    #include "client_stub_mpi.h"
    #include "client_stub_socket.h"
    #include "client_stub_local.h"


    // File System API
    int mfs_api_init     ( comm_t *wb, params_t *params ) ;
    int mfs_api_finalize ( comm_t *wb, params_t *params ) ;

    // File API
    long mfs_api_open    ( comm_t *wb, const char *pathname, int flags ) ;
    int  mfs_api_close   ( comm_t *wb, long fd ) ;
    int  mfs_api_read    ( comm_t *wb, long fd, void *buf, int count ) ;
    int  mfs_api_write   ( comm_t *wb, long fd, void *buf, int count ) ;

    // Directory API
    long mfs_api_mkdir   ( comm_t *wb, const char *pathname, int mode ) ;
    long mfs_api_rmdir   ( comm_t *wb, const char *pathname ) ;

    // DBM File API
    long mfs_api_dbmopen   ( comm_t *wb, const char *pathname, int flags ) ;
    int  mfs_api_dbmclose  ( comm_t *wb, long fd ) ;
    int  mfs_api_dbmstore  ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int  count_val ) ;
    int  mfs_api_dbmfetch  ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val ) ;
    int  mfs_api_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key ) ;

#endif

