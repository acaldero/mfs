
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#ifndef __MFS_API_H__
#define __MFS_API_H__

    // Includes
    #include "base_lib.h"
    #include "mfs_dbm.h"
    #include "client_stub_mpi.h"
    #include "client_stub_socket.h"
    #include "client_stub_local.h"


    // Datatypes
    typedef struct
    {
	// client itself
        comm_t    cb[COMM_NUMBER_OF_PROTOCOLS] ;

	// servers
        conf_t    partitions ;
        comm_t   *wb ;
        int       n_eltos ;

    } mfs_t ;


    // File System API
    int mfs_api_init            ( mfs_t *wb, params_t *params, int *argc, char ***argv ) ;
    int mfs_api_finalize        ( mfs_t *wb, params_t *params ) ;
    int mfs_api_open_partition  ( mfs_t *wb, params_t *params, char *conf_fname ) ;
    int mfs_api_close_partition ( mfs_t *wb, params_t *params ) ;

    // File API
    long mfs_api_open      ( mfs_t *wb, const char *pathname, int flags ) ;
    int  mfs_api_close     ( mfs_t *wb, long fd ) ;
    int  mfs_api_read      ( mfs_t *wb, long fd, void *buf, int count ) ;
    int  mfs_api_write     ( mfs_t *wb, long fd, void *buf, int count ) ;

    // Directory API
    long mfs_api_mkdir     ( mfs_t *wb, const char *pathname, int mode ) ;
    long mfs_api_rmdir     ( mfs_t *wb, const char *pathname ) ;

    // DBM File API
    long mfs_api_dbmopen   ( mfs_t *wb, const char *pathname, int flags ) ;
    int  mfs_api_dbmclose  ( mfs_t *wb, long fd ) ;
    int  mfs_api_dbmstore  ( mfs_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int  count_val ) ;
    int  mfs_api_dbmfetch  ( mfs_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val ) ;
    int  mfs_api_dbmdelete ( mfs_t *wb, long fd, void *buff_key, int count_key ) ;

    // General API
    int  mfs_api_get_rank  ( mfs_t *wb ) ;
    int  mfs_api_get_size  ( mfs_t *wb ) ;

#endif

