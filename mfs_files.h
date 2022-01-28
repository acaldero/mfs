
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
    #define FILE_USE_POSIX  1
    #define FILE_USE_MPI_IO 2


    // Datatypes
    typedef struct
    {
        // underlying protocol
        int  file_protocol ;

        // descriptors
        long     posix_fd ;
	MPI_File mpiio_fd ;

        // some stats
        long n_read_req ;
        long n_write_req ;

    } file_t ;


    // API
    long mfs_file_fd2long ( file_t *fd ) ;
    int  mfs_file_long2fd ( file_t *fd, long fref, int file_protocol ) ;

    int   mfs_file_open   ( file_t *fd, int file_protocol, const char *path_name, int flags ) ;
    int   mfs_file_close  ( file_t *fd ) ;

    int   mfs_file_read   ( file_t *fd, void *buff_data, int count ) ;
    int   mfs_file_write  ( file_t *fd, void *buff_data, int count ) ;

    void *mfs_file_mmap   ( void *addr, size_t size, int protect, int flags, long filedes, off_t offset ) ;
    int   mfs_file_munmap ( void *addr, size_t size ) ;

#endif

