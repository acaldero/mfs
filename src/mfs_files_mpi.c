
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


#include "mfs_files_mpi.h"


/*
 *  File System API
 */

int  mfs_file_mpi_init ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_file_mpi_finalize ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_file_mpi_open  ( MPI_File *fd, const char *path_name )
{
     int ret ;

     ret = MPI_File_open(MPI_COMM_SELF, path_name, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, fd);
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on open('%s') file.\n", path_name) ;
	 return -1 ;
     }

     // Return OK
     return 1 ;
}

int   mfs_file_mpi_close ( MPI_File *fd )
{
     int ret ;

     ret = MPI_File_close(fd) ;
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on close file.\n") ;
	 return -1 ;
     }

     // Return OK/KO
     return 1 ;
}

int   mfs_file_mpi_read   ( MPI_File fd, void *buffer, int buffer_size )
{
     int ret ;
     MPI_Status status ;

     // Read
     ret = MPI_File_read(fd, buffer, buffer_size, MPI_CHAR, &status) ;
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", buffer_size, fd) ;
	 return -1 ;
     }

     // Get number of bytes read...
     MPI_Get_count(&status, MPI_INT, &ret);

     // Return number of bytes
     return ret ;
}

int   mfs_file_mpi_write  ( MPI_File fd, void *buffer, int buffer_size )
{
     int ret ;
     MPI_Status status ;

     // Write
     ret = MPI_File_write(fd, buffer, buffer_size, MPI_CHAR, &status) ;
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes to file '%d'\n", buffer_size, fd) ;
	 return -1 ;
     }

     // Get number of bytes written...
     MPI_Get_count(&status, MPI_INT, &ret);

     // Return number of bytes
     return ret ;
}

