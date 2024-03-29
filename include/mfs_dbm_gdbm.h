
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

#ifndef __MFS_DBM_GDBM_H__
#define __MFS_DBM_GDBM_H__

    // Includes
    #include "base_lib.h"

    #include <sys/file.h>

#ifdef HAVE_GDBM_H
    #define _XOPEN_SOURCE_EXTENDED 1
    #include <gdbm.h>
#else
    #define GDBM_FILE  void *

    #define GDBM_READER    0
    #define GDBM_WRITER    1
    #define GDBM_WRCREAT   2
    #define GDBM_NEWDB     3
    #define GDBM_OPENMASK  7
#endif


    // API
    int  mfs_dbm_gdbm_init     ( void ) ;
    int  mfs_dbm_gdbm_finalize ( void ) ;

    int  mfs_dbm_gdbm_open   ( GDBM_FILE *fd, const char *path_name, int flags ) ;
    int  mfs_dbm_gdbm_close  ( GDBM_FILE  fd ) ;

    int  mfs_dbm_gdbm_store  ( GDBM_FILE  fd, void *buff_key, int count_key, void *buff_val, int  count_val ) ;
    int  mfs_dbm_gdbm_fetch  ( GDBM_FILE  fd, void *buff_key, int count_key, void *buff_val, int *count_val ) ;
    int  mfs_dbm_gdbm_delete ( GDBM_FILE  fd, void *buff_key, int count_key ) ;

#endif

