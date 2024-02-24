
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

#ifndef __MFS_FS_LOG_H__
#define __MFS_FS_LOG_H__

    // Includes
    #include "base_lib.h"
    #include "base_time.h"
    #include "mfs_fs.h"
    #include "mfs_file_posix.h"
    #include "mfs_directory_posix.h"


    // Datatypes
    template <class F = mfs_file_posix, class D = mfs_directory_posix>
    class mfs_fs_log : mfs_fs<F,D>
    {
      protected:

      public:
	 mfs_fs_log ( ) ;
	 mfs_fs_log ( mfs_fs<F,D> low_fs ) ;
	~mfs_fs_log ( ) ;

        // File API
        F        creat ( char *path, mode_t mode ) ;
        F        open  ( char *path, int flags, mode_t mode ) ;
        int      close ( F &fd ) ;
        off_t    lseek ( F &fd, off_t offset, int flag ) ;
        ssize_t  write ( F &fd, void *buffer, size_t size ) ;
        ssize_t  read  ( F &fd, void *buffer, size_t size ) ;

        // Directory API
        D                opendir   ( char *path ) ;
        int              closedir  ( D &dirp ) ;
        struct dirent *  readdir   ( D &dirp ) ;
        void             rewinddir ( D &dirp ) ;

        int     mkdir  ( const char *pathname, int mode ) ;
        int     rmdir  ( const char *pathname ) ;
    } ;

#endif

