
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

#ifndef __MFS_DBM_TDB_H__
#define __MFS_DBM_TDB_H__

    // Includes
    #include "base_lib.h"

    #include <sys/file.h>

#ifdef HAVE_TDB_H
    #include <tdb.h>
#else
    #define TDB_CONTEXT         void *

    #define TDB_CLEAR_IF_FIRST  0
    #define TDB_INTERNAL        1
    #define TDB_NOLOCK          2
    #define TDB_NOMMAP          3
    #define TDB_CONVERT         7
#endif


    // API
    int  mfs_dbm_tdb_init     ( void ) ;
    int  mfs_dbm_tdb_finalize ( void ) ;

    int  mfs_dbm_tdb_open   ( TDB_CONTEXT **fd, const char *path_name, int flags ) ;
    int  mfs_dbm_tdb_close  ( TDB_CONTEXT  *fd ) ;

    int  mfs_dbm_tdb_store  ( TDB_CONTEXT  *fd, void *buff_key, int count_key, void *buff_val, int  count_val ) ;
    int  mfs_dbm_tdb_fetch  ( TDB_CONTEXT  *fd, void *buff_key, int count_key, void *buff_val, int *count_val ) ;
    int  mfs_dbm_tdb_delete ( TDB_CONTEXT  *fd, void *buff_key, int count_key ) ;

#endif

