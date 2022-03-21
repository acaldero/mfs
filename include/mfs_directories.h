
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

#ifndef __MFS_DIRECTORIES_H__
#define __MFS_DIRECTORIES_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_descriptors.h"
    #include "mfs_directories_posix.h"


    // Directory protocol
    #define DIRECTORY_USE_POSIX  1


    // Datatypes
    typedef struct
    {
        int   been_used ;
        int   dir_descriptor ;

        // underlying protocol
        int   directory_backend ;
        char *directory_backend_name ;

        // descriptors
        DIR  *posix_fd ;

    } dir_t ;

    typedef struct dirent dirent_t ;


    // API
    int   mfs_directory_init       ( void ) ;
    int   mfs_directory_finalize   ( void ) ;

    long  mfs_directory_fd2long    ( int  fd ) ;
    int   mfs_directory_long2fd    ( int *fd, long fref, int directory_backend ) ;
    int   mfs_directory_stats_show ( int  fd, char *prefix ) ;

    int   mfs_directory_opendir    ( int *fd, int directory_backend, const char *path_name ) ;
    int   mfs_directory_closedir   ( int  fd ) ;

    int   mfs_directory_readdir    ( int  fd, struct dirent *dent ) ;

    int   mfs_directory_mkdir      ( int directory_backend, char *path_name, mode_t mode ) ;
    int   mfs_directory_rmdir      ( int directory_backend, char *path_name ) ;

#endif

