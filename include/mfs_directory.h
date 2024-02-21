
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

#ifndef __MFS_DIRECTORY_H__
#define __MFS_DIRECTORY_H__

    // Includes
    #include "base_lib.h"

    #include <dirent.h>
    #include <sys/stat.h>

    class mfs_directory
    {
	public:
          virtual int            init     ( void ) = 0 ;
          virtual int            finalize ( void ) = 0 ;
          virtual int            opendir  ( const char *path_name ) = 0 ;
          virtual int            closedir ( void ) = 0 ;
          virtual struct dirent *readdir  ( void ) = 0 ;
          virtual int            mkdir    ( char *path_name, mode_t mode ) = 0 ;
          virtual int            rmdir    ( char *path_name ) = 0 ;
    } ;

#endif

