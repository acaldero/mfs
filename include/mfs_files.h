
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

#ifndef __MFS_FILES_H__
#define __MFS_FILES_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_descriptors.h"
    #include "mfs_files_posix.h"
    #include "mfs_files_mpi.h"


    // File protocol
    #define FILE_USE_POSIX  1
    #define FILE_USE_MPI_IO 2


    // Datatypes
    typedef struct
    {
        // underlying protocol
        int   file_backend ;
        char *file_backend_name ;

        // descriptors
        long          posix_fd ;
	MPI_File      mpiio_fd ;

        // some stats
        long  offset ;
        long  n_read_req ;
        long  n_write_req ;

    } file_t ;


    // API
    int   mfs_file_init       ( void ) ;
    int   mfs_file_finalize   ( void ) ;

    long  mfs_file_fd2long    ( int  fd ) ;
    int   mfs_file_long2fd    ( int *fd, long fref ) ;
    int   mfs_file_stats_show ( int  fd, char *prefix ) ;

    int   mfs_file_open       ( int *fd, int file_backend, const char *path_name, int flags ) ;
    int   mfs_file_close      ( int  fd ) ;

    int   mfs_file_read       ( int  fd, void *buff_data, int count ) ;
    int   mfs_file_write      ( int  fd, void *buff_data, int count ) ;

#endif

