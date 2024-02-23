
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


#include "mfs_fs_log.h"


/*
 *  File System API
 */

template <class F, class D>
mfs_fs_log<F,D>::mfs_fs_log ( )
{
     this->fs_backend = 1 ;
     this->fs_backend_name = "LOG" ;

     // terminal/low-level file system
     this->low_fs = NULL ;
}

template <class F, class D>
mfs_fs_log<F,D>::mfs_fs_log ( mfs_fs<F,D> low_fs )
{
     this->fs_backend = 1 ;
     this->fs_backend_name = "LOG" ;

     // terminal/low-level file system
     this->low_fs = low_fs ;
}

template <class F, class D>
mfs_fs_log<F,D>::~mfs_fs_log ( )
{
     // terminal/low-level file system
     this->low_fs = NULL ;
}


// File API
template <class F, class D>
F   mfs_fs_log<F,D>::creat  ( char *path, mode_t mode )
{
        F fd ;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("path:%s, mode:%d", path, (int)mode) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    fd = this->low_fs.creat(path, mode) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "creat", fd, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("path:%s, mode:%d -> %d", path, (int)mode, &fd) ;

	return fd ;
}

template <class F, class D>
F   mfs_fs_log<F,D>::open   ( char *path, int flags, mode_t mode )
{
        F fd ;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("path:%s, mode:%d", path, (int)mode) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    fd = this->low_fs.open(path, flags, mode) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "open", fd, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("path:%s, mode:%d -> %d", path, (int)mode, &fd) ;

	return fd ;
}

template <class F, class D>
int  mfs_fs_log<F,D>::close  ( F &fd )
{
	int ret = -1;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("fd:%p", (int)&fd) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.close(fd) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "close", fd, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("fd:%p -> %d", (int)&fd, ret) ;

	return ret ;
}

template <class F, class D>
off_t  mfs_fs_log<F,D>::lseek  ( F &fd, off_t offset, int flag )
{
	off_t ret = -1;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("fd:%p", (int)&fd) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.lseek(fd, offset, flag) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "lseek", fd, offset, -1) ;

	//XPN_DEBUG_END_CUSTOM("fd:%p -> %d", (int)&fd, ret) ;

	return ret ;
}

template <class F, class D>
ssize_t mfs_fs_log<F,D>::write  ( F &fd, void *buffer, size_t size )
{
	ssize_t ret = -1;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("fd:%p", (int)&fd) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.write(fd, buffer, size) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "write", fd, buffer, size) ;

	//XPN_DEBUG_END_CUSTOM("fd:%p -> %d", (int)&fd, ret) ;

	return ret ;
}

template <class F, class D>
ssize_t mfs_fs_log<F,D>::read   ( F &fd, void *buffer, size_t size )
{
	ssize_t ret = -1;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("fd:%p", (int)&fd) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.read(fd, buffer, size) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%d;t2:%d;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "read", fd, buffer, size) ;

	//XPN_DEBUG_END_CUSTOM("fd:%p -> %d", (int)&fd, ret) ;

	return ret ;
}


// Directory API
template <class F, class D>
D  mfs_fs_log<F,D>::opendir ( char *path )
{
        D dd ;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("path:%s", path) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    dd = this->low_fs.opendir(path) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%s;offset:%ld;size:%ld\n", t1, t2, "opendir", path, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("path:%s -> %p", path, &dd) ;

	return dd ;
}

template <class F, class D>
int mfs_fs_log<F,D>::closedir  ( D &dd )
{
        int ret ;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("dd:%p", &dd) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.closedir(dd) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "closedir", &dd, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("dd:%p -> %d", &dd, ret) ;

	return ret ;
}

template <class F, class D>
struct dirent * mfs_fs_log<F,D>::readdir  ( D &dd )
{
        struct dirent *ret ;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("dd:%p", &dd) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.readdir(dd) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "readdir", &dd, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("dd:%p -> %p", &dd, ret) ;

	return ret ;
}

template <class F, class D>
void mfs_fs_log<F,D>::rewinddir ( D &dd )
{
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("dd:%p", &dd) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    this->low_fs.rewinddir(dd) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%p;offset:%ld;size:%ld\n", t1, t2, "rewinddir", &dd, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("dd:%p -> %p", &dd, ret) ;
}


template <class F, class D>
int mfs_fs_log<F,D>::mkdir  ( const char *path_name, int mode )
{
        int ret = -1 ;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("path_name:%s", path_name) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.mkdir(path_name, mode) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%s;offset:%ld;size:%ld\n", t1, t2, "mkdir", path_name, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("dd:%p -> %p", &dd, ret) ;

	return ret ;
}

template <class F, class D>
int mfs_fs_log<F,D>::rmdir  ( const char *path_name )
{
        int ret = -1 ;
  	long t1, t2 ;

	//XPN_DEBUG_BEGIN_CUSTOM("path_name:%s", path_name) ;

	/* passthru... */
        t1 = mfs_get_time() ;
        if (NULL != this->low_fs) {
	    ret = this->low_fs.rmdir(path_name) ;
	}
        t2 = mfs_get_time() ;

        /* record event: t1, t2, action, object, offset, size */
        printf("t1:%ld;t2:%ld;action:%s;object:%s;offset:%ld;size:%ld\n", t1, t2, "rmdir", path_name, -1, -1) ;

	//XPN_DEBUG_END_CUSTOM("dd:%p -> %p", &dd, ret) ;

	return ret ;
}


