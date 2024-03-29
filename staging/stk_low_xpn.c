
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

      #include "include/stk_low_xpn.h"


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int stk_low_xpn_init ( stk_fs_t *fsi, stk_fs_t *low_fsi )
      {
	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p, low_fsi:%p", fsi, low_fsi) ;

	  /* check params */
	  if (NULL == fsi) {
	      return -1 ;
	  }

	  /* register stk_low_xpn interface */
	  fsi->fsi_name   = STRING_MISC_StrDup("stk_low_xpn") ;
	  fsi->low_fsi    = low_fsi ;

	  // init/finalize
	  fsi->fsi_init       = stk_low_xpn_init ;
          fsi->fsi_finalize   = stk_low_xpn_finalize ;
	  // file API
          fsi->fsi_open       = stk_low_xpn_open ;
          fsi->fsi_creat      = stk_low_xpn_creat ;
          fsi->fsi_close      = stk_low_xpn_close ;
          fsi->fsi_read       = stk_low_xpn_read ;
          fsi->fsi_write      = stk_low_xpn_write ;
          fsi->fsi_lseek      = stk_low_xpn_lseek ;
	  // directory API
          fsi->fsi_opendir    = stk_low_xpn_opendir ;
          fsi->fsi_closedir   = stk_low_xpn_closedir ;
          fsi->fsi_readdir    = stk_low_xpn_readdir ;
          fsi->fsi_mkdir      = stk_low_xpn_mkdir ;
          fsi->fsi_rmdir      = stk_low_xpn_rmdir ;
          fsi->fsi_rewinddir  = stk_low_xpn_rewinddir ;

	  XPN_DEBUG_END_CUSTOM("fsi:%p, low_fsi:%p -> %d", fsi, low_fsi, ret) ;

	  /* return ok */
	  return xpn_init() ;
      }

      int stk_low_xpn_finalize ( stk_fs_t *fsi )
      {
	  /* check params */
	  if (NULL == fsi) {
	      return -1 ;
	  }

	  /* unregister stk_low_xpn interface */
	  if (NULL != fsi->fsi_name) {
	      free(fsi->fsi_name) ;
	      fsi->fsi_name   = NULL ;
	  }

	  // init/finalize
	  fsi->fsi_init       = NULL ;
          fsi->fsi_finalize   = NULL ;
	  // file API
          fsi->fsi_open       = NULL ;
          fsi->fsi_creat      = NULL ;
          fsi->fsi_close      = NULL ;
          fsi->fsi_read       = NULL ;
          fsi->fsi_write      = NULL ;
          fsi->fsi_lseek      = NULL ;
	  // directory API
          fsi->fsi_opendir    = NULL ;
          fsi->fsi_closedir   = NULL ;
          fsi->fsi_readdir    = NULL ;
          fsi->fsi_mkdir      = NULL ;
          fsi->fsi_rmdir      = NULL ;
          fsi->fsi_rewinddir  = NULL ;

	  /* return ok */
	  return xpn_finalize() ;
      }


      //
      // open + close + sread + swrite
      //

      int stk_low_xpn_creat ( char *path, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	      return -1 ;
	  }

          /* create file */
          return xpn_creat(path, mode) ;
      }

      int stk_low_xpn_open ( char *path, int flags, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	     return -1 ;
	  }

          /* open file */
          return xpn_open(path, flags, mode) ;
      }

      int stk_low_xpn_close ( int fd )
      {
	  return xpn_close(fd) ;
      }

      off_t  stk_low_xpn_lseek ( int fd, off_t offset, int flag )
      {
	  return xpn_lseek(fd, offset, flag) ;
      }

      ssize_t stk_low_xpn_write ( int fd, void *buffer, size_t size )
      {
	  return xpn_pwrite(fd, offset, flag) ; // TODO
      }

      ssize_t stk_low_xpn_read ( int fd, void *buffer, size_t size )
      {
	  return xpn_pread(fd, offset, flag) ; // TODO
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * stk_low_xpn_opendir ( char *path )
      {
	  return xpn_opendir(path) ;
      }

      int     stk_low_xpn_closedir  ( DIR *dirp )
      {
	  return xpn_closedir(dirp) ;
      }

      struct dirent *stk_low_xpn_readdir ( DIR *dirp )
      {
	  return xpn_readdir(dirp) ;
      }

      void    stk_low_xpn_rewinddir ( DIR *dirp )
      {
	  return xpn_rewinddir(dirp) ;
      }


      long    stk_low_xpn_mkdir     ( const char *pathname, int mode )
      {
	  return xpn_mkdir(pathname, mode) ;
      }

      long    stk_low_xpn_rmdir     ( const char *pathname )
      {
	  return xpn_rmdir(pathname) ;
      }


   /* ................................................................... */

