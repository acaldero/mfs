
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

#ifndef __MFS_FILES_POSIX_H__
#define __MFS_FILES_POSIX_H__

    // Includes
    #include "base_lib.h"

    #include <sys/stat.h>
    #include <sys/mman.h>


    // API
    int  mfs_file_posix_init     ( void ) ;
    int  mfs_file_posix_finalize ( void ) ;

    int  mfs_file_posix_open  ( long *fd, const char *path_name, int flags ) ;
    int  mfs_file_posix_close ( int fd ) ;
    int  mfs_file_posix_read  ( int fd, void *buffer, int buffer_size ) ;
    int  mfs_file_posix_write ( int fd, void *buffer, int buffer_size ) ;

#endif

