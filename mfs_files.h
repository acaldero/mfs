
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

#ifndef __MFS_FILES_H__
#define __MFS_FILES_H__

    // Includes
    #include "mpi.h"
    #include "mfs_lib.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/mman.h>


    // File protocol
    #define USE_POSIX  1
    #define USE_MPI_IO 2


    // API
    int   server_files_open  ( long *fd, int file_protocol, const char *path_name, int flags ) ;
    int   server_files_close ( long  fd, int file_protocol ) ;

    int   server_files_read  ( long  fd, int file_protocol, void *buff_char, int count ) ;
    int   server_files_write ( long  fd, int file_protocol, void *buff_char, int count ) ;

    void *server_files_mmap   ( void *addr, size_t size, int protect, int flags, long filedes, off_t offset ) ;
    int   server_files_munmap ( void *addr, size_t size ) ;

#endif

