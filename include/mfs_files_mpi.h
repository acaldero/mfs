
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

#ifndef __MFS_FILES_MPI_H__
#define __MFS_FILES_MPI_H__

    // Includes
    #include "base_lib.h"

    #include <mpi.h>
    #include <fcntl.h>


    // API
    int  mfs_file_mpi_init     ( void ) ;
    int  mfs_file_mpi_finalize ( void ) ;

    int  mfs_file_mpi_open  ( MPI_File *fd, const char *path_name ) ;
    int  mfs_file_mpi_close ( MPI_File *fd ) ;
    int  mfs_file_mpi_read  ( MPI_File  fd, void *buffer, int buffer_size ) ;
    int  mfs_file_mpi_write ( MPI_File  fd, void *buffer, int buffer_size ) ;

#endif

