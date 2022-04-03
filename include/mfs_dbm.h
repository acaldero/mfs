
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

#ifndef __MFS_DBM_H__
#define __MFS_DBM_H__

    // Includes
    #include "base_lib.h"
    #include "mfs_descriptors.h"
    #include "mfs_dbm_gdbm.h"
    #include "mfs_dbm_redis.h"


    // File protocol
    #define DBM_USE_GDBM   1
    #define DBM_USE_REDIS  2


    // Datatypes
    typedef struct
    {
        int   been_used ;
        int   dbm_fd ;

        // underlying protocol
        int   dbm_backend ;
        char *dbm_backend_name ;

        // descriptors
        GDBM_FILE     gdbm_fd ;
	redisContext *redis_ctxt ;

        // some stats
        long  offset ;
        long  n_read_req ;
        long  n_write_req ;

    } dbm_t ;


    // API
    int   mfs_dbm_init       ( void ) ;
    int   mfs_dbm_finalize   ( void ) ;

    long  mfs_dbm_fd2long    ( int  fd ) ;
    int   mfs_dbm_long2fd    ( int *fd, long fref, int file_backend ) ;
    int   mfs_dbm_stats_show ( int  fd, char *prefix ) ;

    int   mfs_dbm_open       ( int *fd, int file_backend, const char *path_name, int flags ) ;
    int   mfs_dbm_close      ( int  fd ) ;

    int   mfs_dbm_store      ( int  fd, void *buff_key, int count_key, void  *buff_val, int  count_val ) ;
    int   mfs_dbm_fetch      ( int  fd, void *buff_key, int count_key, void **buff_val, int *count_val ) ;
    int   mfs_dbm_delete     ( int  fd, void *buff_key, int count_key ) ;

#endif

