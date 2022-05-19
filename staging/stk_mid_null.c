
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
      // register + unregister
      //

      int stk_mid_null_register   ( xpni_fsi_t *fsi )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%p", fsi) ;

	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* register stk_mid_null interface */
	fsi->fsi_name   = STRING_MISC_StrDup("stk_mid_null") ;

	// file API
	fsi->fsi_init       = xpnsi_null_init ;
        fsi->fsi_destroy    = xpnsi_null_destroy ;
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
	// register/unregister API
        fsi->fsi_register   = xpnsi_null_register ;
        fsi->fsi_unregister = xpnsi_null_unregister ;

	XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	/* return ok */
	return (1) ;
      }

      int stk_mid_null_unregister ( xpni_fsi_t *fsi )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%p", fsi) ;

	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* unregister xpnsi_null interface */
	if (NULL != fsi->fsi_name) {
	    free(fsi->fsi_name) ;
	    fsi->fsi_name   = NULL ;
	}

	// file API
	fsi->fsi_init       = NULL ;
        fsi->fsi_destroy    = NULL ;
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
	// register/unregister API
        fsi->fsi_register   = NULL ;
        fsi->fsi_unregister = NULL ;

	XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	/* return ok */
	return (1) ;
      }


      //
      // init + destroy
      //

      int stk_mid_null_init ()
      {
	int ret ;

	XPN_DEBUG_BEGIN();
	ret = xpni_lowfsi_init();
	XPN_DEBUG_END();

	return ret ;
      }

      int stk_mid_null_destroy ()
      {
	int ret ;

	XPN_DEBUG_BEGIN();
	ret = xpni_lowfsi_destroy();
	XPN_DEBUG_END();

	return ret ;
      }


      //
      // open + close + sread + swrite
      //

      int stk_mid_null_creat ( int fd )
      {
        xpni_fi_t *xpni_fi1 ;
        int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

        /* check params */
        xpni_fi1 = xpni_fit_get(fd) ;
        if (NULL == xpni_fi1) {
	    return -1 ;
	}

        /* create file */
        ret = xpni_lowfsi_creat(xpni_fit_get_XPN_FNAME(fd),
                                xpni_fit_get_XPN_MODE(fd)) ;
        if (ret < 0) {
	    return -1 ;
        }

        /* update file description */
        xpni_fit_set_XPN_DATA_FD(fd,ret) ;

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return xpn file descriptor */
        return ret ;
      }

      int stk_mid_null_open ( int fd )
      {
        xpni_fi_t *xpni_fi1 ;
        int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

        /* check params */
        xpni_fi1 = xpni_fit_get(fd) ;
        if (NULL == xpni_fi1) {
	   return -1 ;
	}

        /* open file */
        ret = xpni_lowfsi_open(xpni_fit_get_XPN_FNAME(fd),
                               xpni_fit_get_XPN_FLAG(fd),
                               xpni_fit_get_XPN_MODE(fd)) ;
        if (ret < 0) {
	    return -1 ;
        }

        /* update file description */
        xpni_fit_set_XPN_DATA_FD(fd,ret) ;

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return xpn file descriptor */
        return ret ;
      }

      int stk_mid_null_close ( int fd )
      {
	int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;
	ret = xpni_lowfsi_close(xpni_fit_get_XPN_DATA_FD(fd));
	XPN_DEBUG_END_CUSTOM("%d", fd) ;

	return ret ;
      }

      off_t  stk_mid_null_lseek ( int fd, off_t offset, int flag )
      {
	int ret ;

        /* lseek file: data only */
	XPN_DEBUG_BEGIN_CUSTOM("%d %d %d", fd, offset, flag) ;
	ret = xpni_lowfsi_lseek(xpni_fit_get_XPN_DATA_FD(fd),offset,flag);
	XPN_DEBUG_END_CUSTOM("%d %d %d", fd, offset, flag) ;

	return ret ;
      }

      ssize_t stk_mid_null_write ( int fd, void *buffer, size_t size )
      {
	int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;
	ret = xpni_lowfsi_write(xpni_fit_get_XPN_DATA_FD(fd), buffer, size);
	XPN_DEBUG_END_CUSTOM("%d %p %d", fd, buffer, size) ;

	return ret ;
      }

      ssize_t stk_mid_null_read ( int fd, void *buffer, size_t size )
      {
	int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;
	ret = xpni_lowfsi_read(xpni_fit_get_XPN_DATA_FD(fd), buffer, size);
	XPN_DEBUG_END_CUSTOM("%d %p %d", fd, buffer, size) ;

	return ret ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * stk_mid_null_opendir ( char * path )
      {
        DIR  *ret;

	XPN_DEBUG_BEGIN_CUSTOM("%s", path) ;
	ret = xpni_lowfsi_opendir(xpni_fit_get_XPN_FNAME(path));
	XPN_DEBUG_END_CUSTOM("%s", path) ;

	return ret;
      }

      int     xpnsi_null_closedir  ( DIR *dirp )
      {
        int  ret;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;
	ret = xpni_lowfsi_closedir(dirp);
	XPN_DEBUG_END_CUSTOM("%p", dirp) ;

	return ret;
      }

      struct dirent *xpnsi_null_readdir ( DIR *dirp )
      {
        struct dirent *ret;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;
	ret = xpni_lowfsi_readdir(dirp);
	XPN_DEBUG_END_CUSTOM("%p", dirp) ;

	return ret;
      }

      void    xpnsi_null_rewinddir ( DIR *dirp )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;
	xpni_lowfsi_readdir(dirp);
	XPN_DEBUG_END_CUSTOM("%p", dirp) ;
      }


      long    xpnsi_null_mkdir     ( const char *pathname, int mode )
      {
        long  ret;

	XPN_DEBUG_BEGIN_CUSTOM("%s %d", pathname, mode) ;
	ret = xpni_lowfsi_mkdir(pathname, mode);
	XPN_DEBUG_END_CUSTOM("%s %d", pathname, mode) ;

	return ret;
      }

      long    xpnsi_null_rmdir     ( const char *pathname )
      {
        long  ret;

	XPN_DEBUG_BEGIN_CUSTOM("%s", pathname) ;
	ret = xpni_lowfsi_rmdir(pathname);
	XPN_DEBUG_END_CUSTOM("%s", pathname) ;

	return ret;
      }


   /* ................................................................... */

