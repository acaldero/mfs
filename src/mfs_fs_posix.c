
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


#include "mfs_fs_posix.h"


/*
 *  File System API
 */

template <class F, class D>
mfs_fs_posix<F,D>::mfs_fs_posix ( )
{
     this->fs_backend = 1 ;
     this->fs_backend_name = "POSIX" ;
}

template <class F, class D>
mfs_fs_posix<F,D>::~mfs_fs_posix ( )
{
}


// File API
template <class F, class D>
F   mfs_fs_posix<F,D>::creat  ( char *path, mode_t mode )
{
        F fd ;

	int ret = fd.creat(path, mode) ;
	return fd ;
}

template <class F, class D>
F   mfs_fs_posix<F,D>::open   ( char *path, int flags, mode_t mode )
{
        F fd ;

	int ret = fd.open(path, flags, mode) ;
	return fd ;
}

template <class F, class D>
int  mfs_fs_posix<F,D>::close  ( F &fd )
{
	return fd.close() ;
}

template <class F, class D>
off_t  mfs_fs_posix<F,D>::lseek  ( F &fd, off_t offset, int flag )
{
	return fd.lseek(offset, flag) ;
}

template <class F, class D>
ssize_t mfs_fs_posix<F,D>::write  ( F &fd, void *buffer, size_t size )
{
	return fd.write(buffer, size) ;
}

template <class F, class D>
ssize_t mfs_fs_posix<F,D>::read   ( F &fd, void *buffer, size_t size )
{
	return fd.read(buffer, size) ;
}


// Directory API
template <class F, class D>
D  mfs_fs_posix<F,D>::opendir ( char *path )
{
        D fd ;

	int ret = fd.opendir(path) ;
	return fd ;
}

template <class F, class D>
int mfs_fs_posix<F,D>::closedir  ( D &dd )
{
	return dd.closedir() ;
}

template <class F, class D>
struct dirent * mfs_fs_posix<F,D>::readdir  ( D &dd )
{
	return dd.readdir() ;
}

template <class F, class D>
void mfs_fs_posix<F,D>::rewinddir ( D &dd )
{
	dd.rewinddir() ;
}


template <class F, class D>
int mfs_fs_posix<F,D>::mkdir  ( const char *path_name, int mode )
{
    int ret = -1 ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(path_name, "[DIR_POSIX] NULL path_name :-/", -1) ;

    // mkdir directory...
    ret = ::mkdir(path_name, mode) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on mkdir directory '%s'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

template <class F, class D>
int mfs_fs_posix<F,D>::rmdir  ( const char *path_name )
{
    int ret = -1 ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(path_name, "[DIR_POSIX] NULL path_name :-/", -1) ;

    // Read from file...
    ret = ::rmdir(path_name) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on rmdir directory '%s'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}


