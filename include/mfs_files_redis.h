
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

#ifndef __MFS_FILES_REDIS_H__
#define __MFS_FILES_REDIS_H__

    // Includes
    #include "mfs_lib.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>

#ifdef HAVE_HIREDIS_H
    #include <hiredis/hiredis.h>
#else
    #define redisContext int
#endif


    // API
    int  mfs_file_redis_init     ( void ) ;
    int  mfs_file_redis_finalize ( void ) ;

    int  mfs_file_redis_open  ( redisContext **red_ctxt, char **red_key, const char *path_name ) ;
    int  mfs_file_redis_close ( redisContext  *red_ctxt, char **red_key ) ;
    int  mfs_file_redis_read  ( redisContext  *red_ctxt, char  *red_key, long *offset, void *buffer, int buffer_size ) ;
    int  mfs_file_redis_write ( redisContext  *red_ctxt, char  *red_key, long *offset, void *buffer, int buffer_size ) ;

#endif

