
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


#include "mfs_file_mpi.h"


/*
 *  File System API
 */

int  mfs_file_mpi::open  ( const char *path_name )
{
     int ret ;

     ret = MPI_File_open(MPI_COMM_SELF, path_name, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &(this->fd)) ;
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on open('%s') file.\n", path_name) ;
	 return -1 ;
     }

     this->file_backend = 1 ;
     this->file_backend_name = "MPI-IO" ;

     // Return OK
     return 1 ;
}

int   mfs_file_mpi::close ( void )
{
     int ret ;

     ret = MPI_File_close(&(this->fd)) ;
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on close file.\n") ;
	 return -1 ;
     }

     this->file_backend = 0 ;
     this->file_backend_name = "" ;

     // Return OK/KO
     return 1 ;
}

int   mfs_file_mpi::read   ( void *buffer, int buffer_size )
{
     int ret ;
     MPI_Status status ;

     // Read
     ret = MPI_File_read(this->fd, buffer, buffer_size, MPI_CHAR, &status) ;
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", buffer_size, fd) ;
	 return -1 ;
     }

     // Get number of bytes read...
     MPI_Get_count(&status, MPI_INT, &ret);

     // Stats...
     (this->n_read_req) ++ ;

     // Return number of bytes
     return ret ;
}

int   mfs_file_mpi::write  ( void *buffer, int buffer_size )
{
     int ret ;
     MPI_Status status ;

     // Write
     ret = MPI_File_write(this->fd, buffer, buffer_size, MPI_CHAR, &status) ;
     if (ret != MPI_SUCCESS) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes to file '%d'\n", buffer_size, fd) ;
	 return -1 ;
     }

     // Get number of bytes written...
     MPI_Get_count(&status, MPI_INT, &ret);

     // Stats...
     (this->n_write_req) ++ ;

     // Return number of bytes
     return ret ;
}

int  mfs_file_mpi::stats_show ( char *prefix )
{
    // Print stats...
    printf("%s: File:\n",            prefix) ;
    printf("%s: + been_used=1\n",    prefix) ;
    printf("%s: + file_fd=%d\n",     prefix, this->fd) ;
    printf("%s: + protocol=%s\n",    prefix, this->file_backend_name) ;
    printf("%s: + # read=%ld\n",     prefix, this->n_read_req) ;
    printf("%s: + # write=%ld\n",    prefix, this->n_write_req) ;

    // Return OK
    return 1 ;
}


