
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

#ifndef __MFS_DIRECTORIES_RED_H__
#define __MFS_DIRECTORIES_RED_H__

    // Includes
    #include "mfs_lib.h"
    #include <hiredis/hiredis.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>


    // API
    int   mfs_directory_redis_init     ( void ) ;
    int   mfs_directory_redis_finalize ( void ) ;
    int   mfs_directory_redis_opendir  ( redisContext **red_ctxt, char **red_key, const char *path_name ) ;
    int   mfs_directory_redis_closedir ( redisContext  *red_ctxt, char **red_key ) ;
    int   mfs_directory_redis_readdir  ( redisContext  *red_ctxt, char  *red_key, void *buffer, int buffer_size ) ;
    int   mfs_directory_redis_mkdir    ( redisContext  *red_ctxt, char  *red_key, char *path_name, mode_t mode ) ;
    int   mfs_directory_redis_rmdir    ( redisContext  *red_ctxt, char  *red_key, char *path_name ) ;

#endif

