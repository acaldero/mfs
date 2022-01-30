
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

#ifndef __MFS_DIRECTORIES_H__
#define __MFS_DIRECTORIES_H__

    // Includes
    #include "mpi.h"
    #include "mfs_lib.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <dirent.h>
    #include <sys/stat.h>


    // Directory protocol
    #define DIRECTORY_USE_POSIX  1


    // Datatypes
    typedef struct
    {
        // underlying protocol
        int  file_protocol ;

        // descriptors
        DIR *posix_fd ;

    } dir_t ;

    typedef struct dirent dirent_t ;


    // API
    long      mfs_directory_fd2long    ( dir_t *fd ) ;
    int       mfs_directory_long2fd    ( dir_t *fd, long fref, int file_protocol ) ;
    int       mfs_directory_stats_show ( dir_t *fd, char *prefix ) ;

    int       mfs_directory_opendir  ( dir_t *fd, int file_protocol, const char *path_name ) ;
    int       mfs_directory_closedir ( dir_t *fd ) ;
    dirent_t *mfs_directory_readdir  ( dir_t *fd ) ;
    int       mfs_directory_mkdir    ( char *path_name, mode_t mode ) ;
    int       mfs_directory_rmdir    ( char *path_name ) ;

#endif

