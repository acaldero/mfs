
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


#include "mfs_files_posix.h"


/*
 *  File System API
 */

int  mfs_file_posix_open ( const char *path_name, int flags )
{
     int  ret ;

     // Check params...
     ret = open(path_name, flags, 0755) ;
     if (ret < 0) {
 	 return -1 ;
     }

     // Return file descriptor
     return ret ;
}

int   mfs_file_posix_close ( int fd )
{
     int ret ;

     // Close file
     ret = close(fd) ;

     // Return OK/KO
     return ret ;
}

int   mfs_file_posix_read   ( int fd, void *buffer, int buffer_size )
{
     ssize_t bytes_read ;
     ssize_t remaining_bytes ;

     remaining_bytes = buffer_size ;
     while (remaining_bytes > 0)
     {
         bytes_read = read(fd, buffer, remaining_bytes) ;
         if (bytes_read < 0) {
	     return -1 ;
         }
         if (bytes_read == 0) {
	     return (buffer_size - remaining_bytes) ;
         }

         remaining_bytes -= bytes_read ;
         buffer          += bytes_read ;
     }

     return buffer_size ;
}

int   mfs_file_posix_write   ( int fd, void *buffer, int buffer_size )
{
     ssize_t write_num_bytes ;
     ssize_t remaining_bytes ;

     remaining_bytes = buffer_size ;
     while (remaining_bytes > 0)
     {
         write_num_bytes = write(fd, buffer, remaining_bytes) ;
         if (write_num_bytes == -1) {
	     return -1 ;
         }

         remaining_bytes -= write_num_bytes ;
         buffer          += write_num_bytes ;
     }

     return buffer_size ;
}
