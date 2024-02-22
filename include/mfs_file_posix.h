
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

#ifndef __MFS_FILE_POSIX_H__
#define __MFS_FILE_POSIX_H__

    // Includes
    #include "base_lib.h"
    #include "mfs_file.h"

    #include <sys/stat.h>
    #include <sys/mman.h>


    class mfs_file_posix : mfs_file
    {
	protected:
	   int fd ;

	public:
           int  open  ( const char *path_name, int flags, int mode ) ;
           int  close ( void ) ;
           int  read  ( void *buffer, int buffer_size ) ;
           int  write ( void *buffer, int buffer_size ) ;
           int  stats_show ( char *prefix ) ;
    } ;

#endif

