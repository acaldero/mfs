
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


   /* ... Include / Inclusion ........................................... */

      #include "include/stk_mid_null.h"


   /* ... Functions / Funciones ......................................... */

      //
      // Init + finalize
      //

      int stk_mid_null_init ( stk_fs_t *fsi, stk_fs_t *low_fsi )
      {
	  int ret ;

	  /* debug */
	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p, low_fsi:%p", fsi, low_fsi) ;

	  /* check params */
	  if (NULL == fsi) {
	      return -1 ;
	  }

	  /* register stk_mid_null interface */
	  fsi->fsi_name       = STRING_MISC_StrDup("stk_mid_null") ;
	  fsi->low_fsi        = low_fsi ;

	  // init/finalize
	  fsi->fsi_init       = xpnsi_null_init ;
          fsi->fsi_finalize   = xpnsi_null_finalize ;
	  // file API
          fsi->fsi_open       = xpnsi_null_open ;
          fsi->fsi_creat      = xpnsi_null_creat ;
          fsi->fsi_close      = xpnsi_null_close ;
          fsi->fsi_read       = xpnsi_null_read ;
          fsi->fsi_write      = xpnsi_null_write ;
          fsi->fsi_lseek      = xpnsi_null_lseek ;
	  // directory API
          fsi->fsi_opendir    = xpnsi_null_opendir ;
          fsi->fsi_closedir   = xpnsi_null_closedir ;
          fsi->fsi_readdir    = xpnsi_null_readdir ;
          fsi->fsi_mkdir      = xpnsi_null_mkdir ;
          fsi->fsi_rmdir      = xpnsi_null_rmdir ;
          fsi->fsi_rewinddir  = xpnsi_null_rewinddir ;

	  // TODO: check if stack builder should do this instead
	  ret = STK_FS_INIT(fsi->low_fsi, NULL) ;

	  /* debug */
	  XPN_DEBUG_END_CUSTOM("fsi:%p, low_fsi:%p -> %d", fsi, low_fsi, ret) ;

	  /* return ok|ko */
	  return ret ;
      }

      int stk_mid_null_finalize ( stk_fs_t *fsi )
      {
	  int       ret ;
          stk_fs_t *low_fsi ;

	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p", fsi) ;

	  // TODO: check if stack builder should do this instead
          low_fsi = STK_FS_GET_LOWFSI(fsi) ;
	  if (NULL != low_fsi) {
              ret = STK_FS_FINALIZE(low_fsi) ;
	  }

	  // finalize
          ret = -1 ;
	  if (NULL != fsi) {
              ret = stk_fs_finalize(fsi) ;
	  }

	  XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	  /* return ok|ko */
	  return ret ;
      }


      //
      // open + close + read + write
      //

      int stk_mid_null_creat ( stk_fs_t *fsi, char *path, mode_t mode )
      {
          int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("path:%s, mode:%d", path, (int)mode) ;

          /* create file */
	  ret = -1 ;
          if (NULL != path) {
              ret = xpni_lowfsi_creat(path, mode) ;
	  }

	  XPN_DEBUG_END_CUSTOM("path:%s, mode:%d -> %d", path, (int)mode, ret) ;

          /* return file descriptor */
          return ret ;
      }

      int stk_mid_null_open ( stk_fs_t *fsi, char *path, int flags, mode_t mode )
      {
          int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("path:%s, flags:%d, mode:%d", path, flags, (int)mode) ;

          /* open file */
	  ret = -1 ;
          if (NULL != path) {
              ret = xpni_lowfsi_open(path, flags, mode) ;
	  }

	  XPN_DEBUG_END_CUSTOM("path:%s, flags:%d, mode:%d -> %d", path, flags, (int)mode, ret) ;

          /* return file descriptor */
          return ret ;
      }

      int stk_mid_null_close ( stk_fs_t *fsi, int fd )
      {
	  int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("fd:%d", fd) ;
	  ret = xpni_lowfsi_close(xpni_fit_get_XPN_DATA_FD(fd));
	    XPN_DEBUG_END_CUSTOM("fd:%d", fd) ;

	  return ret ;
      }

      off_t  stk_mid_null_lseek ( stk_fs_t *fsi, int fd, off_t offset, int flag )
      {
	  int ret ;

          /* lseek file: data only */
	  XPN_DEBUG_BEGIN_CUSTOM("fd:%d, offset:%ld, flag:%d", fd, (long)offset, flag) ;
	  ret = xpni_lowfsi_lseek(xpni_fit_get_XPN_DATA_FD(fd),offset,flag);
	    XPN_DEBUG_END_CUSTOM("fd:%d, offset:%ld, flag:%d", fd, (long)offset, flag) ;

	  return ret ;
      }

      ssize_t stk_mid_null_write ( stk_fs_t *fsi, int fd, void *buffer, size_t size )
      {
	  int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("fd:%d, buffer:%p, size:%d", fd, buffer, size) ;
	  ret = xpni_lowfsi_write(xpni_fit_get_XPN_DATA_FD(fd), buffer, size);
	    XPN_DEBUG_END_CUSTOM("fd:%d, buffer:%p, size:%d", fd, buffer, size) ;

	  return ret ;
      }

      ssize_t stk_mid_null_read ( stk_fs_t *fsi, int fd, void *buffer, size_t size )
      {
	  int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("fd:%d, buffer:%p, size:%d", fd, buffer, size) ;
	  ret = xpni_lowfsi_read(xpni_fit_get_XPN_DATA_FD(fd), buffer, size);
	    XPN_DEBUG_END_CUSTOM("fd:%d, buffer:%p, size:%d", fd, buffer, size) ;

	  return ret ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * stk_mid_null_opendir ( stk_fs_t *fsi, char * path )
      {
          DIR  *ret;

	  XPN_DEBUG_BEGIN_CUSTOM("path:%s", path) ;
	  ret = xpni_lowfsi_opendir(xpni_fit_get_XPN_FNAME(path));
	    XPN_DEBUG_END_CUSTOM("path:%s", path) ;

	  return ret;
      }

      int     xpnsi_null_closedir  ( stk_fs_t *fsi, DIR *dirp )
      {
          int  ret;

	  XPN_DEBUG_BEGIN_CUSTOM("dirp:%p", dirp) ;
	  ret = xpni_lowfsi_closedir(dirp);
	    XPN_DEBUG_END_CUSTOM("dirp:%p", dirp) ;

	  return ret;
      }

      struct dirent *xpnsi_null_readdir ( stk_fs_t *fsi, DIR *dirp )
      {
          struct dirent *ret;

	  XPN_DEBUG_BEGIN_CUSTOM("dirp:%p", dirp) ;
	  ret = xpni_lowfsi_readdir(dirp);
	    XPN_DEBUG_END_CUSTOM("dirp:%p", dirp) ;

	  return ret;
      }

      void    xpnsi_null_rewinddir ( stk_fs_t *fsi, DIR *dirp )
      {
	  XPN_DEBUG_BEGIN_CUSTOM("dirp:%p", dirp) ;
	  xpni_lowfsi_readdir(dirp);
	    XPN_DEBUG_END_CUSTOM("dirp:%p", dirp) ;
      }


      long    xpnsi_null_mkdir     ( stk_fs_t *fsi, const char *pathname, int mode )
      {
          long  ret;

	  XPN_DEBUG_BEGIN_CUSTOM("pathname:%s, mode:%d", pathname, mode) ;
	  ret = xpni_lowfsi_mkdir(pathname, mode);
	    XPN_DEBUG_END_CUSTOM("pathname:%s, mode:%d", pathname, mode) ;

	  return ret;
      }

      long    xpnsi_null_rmdir     ( stk_fs_t *fsi, const char *pathname )
      {
          long  ret;

	  XPN_DEBUG_BEGIN_CUSTOM("pathname:%s", pathname) ;
	  ret = xpni_lowfsi_rmdir(pathname);
	    XPN_DEBUG_END_CUSTOM("pathname:%s", pathname) ;

	  return ret;
      }


   /* ................................................................... */

