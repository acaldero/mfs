
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

#ifndef __MFS_DBM_REDIS_H__
#define __MFS_DBM_REDIS_H__

    // Includes
    #include "base_lib.h"

#ifdef HAVE_HIREDIS_H
    #include <hiredis/hiredis.h>
#else
    #define redisContext int
#endif


    // API
    int  mfs_dbm_redis_init     ( void ) ;
    int  mfs_dbm_redis_finalize ( void ) ;

    int  mfs_dbm_redis_open   ( redisContext **fd, const char *path_name, int flags ) ;
    int  mfs_dbm_redis_close  ( redisContext  *fd ) ;

    int  mfs_dbm_redis_store  ( redisContext  *fd, void *buff_key, int count_key, void  *buff_val, int  count_val ) ;
    int  mfs_dbm_redis_fetch  ( redisContext  *fd, void *buff_key, int count_key, void *buff_val, int *count_val ) ;
    int  mfs_dbm_redis_delete ( redisContext  *fd, void *buff_key, int count_key ) ;

#endif

