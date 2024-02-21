
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


#include "mfs_file_posix.h"


/*
 *  File System API
 */

int  mfs_file_posix::open  ( const char *path_name, int flags )
{
     int  ret ;

     // Check params...
     this->fd = ::open(path_name, flags, 0755) ;
     if (this->fd < 0) {
 	 return -1 ;
     }

     this->file_backend = 2 ;
     this->file_backend_name = "POSIX" ;

     // Return OK
     return 1 ;
}

int   mfs_file_posix::close ( void )
{
     int ret ;

     // Close file
     ret = ::close(this->fd) ;

     this->file_backend = 0 ;
     this->file_backend_name = "" ;

     // Return OK/KO
     return ret ;
}

int   mfs_file_posix::read   ( void *buffer, int buffer_size )
{
     ssize_t bytes_read ;
     ssize_t remaining_bytes ;

     remaining_bytes = buffer_size ;
     while (remaining_bytes > 0)
     {
         bytes_read = ::read(this->fd, buffer, remaining_bytes) ;
         if (bytes_read < 0) {
	     return -1 ;
         }
         if (bytes_read == 0) {
	     return (buffer_size - remaining_bytes) ;
         }

         remaining_bytes -= bytes_read ;
         buffer          += bytes_read ;
     }

     // Stats...
     (this->n_read_req) ++ ;

     return buffer_size ;
}

int   mfs_file_posix::write  ( void *buffer, int buffer_size )
{
     ssize_t write_num_bytes ;
     ssize_t remaining_bytes ;

     remaining_bytes = buffer_size ;
     while (remaining_bytes > 0)
     {
         write_num_bytes = ::write(this->fd, buffer, remaining_bytes) ;
         if (write_num_bytes == -1) {
	     return -1 ;
         }

         remaining_bytes -= write_num_bytes ;
         buffer          += write_num_bytes ;
     }

     // Stats...
     (this->n_write_req) ++ ;

     return buffer_size ;
}

int  mfs_file_posix::stats_show ( char *prefix )
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


