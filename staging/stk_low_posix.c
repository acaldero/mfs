
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

      #include "include/stk_low_posix.h"


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int stk_low_posix_register   ( xpni_fsi_t *fsi )
      {
	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* register stk_low_posix interface */
	fsi->fsi_name   = STRING_MISC_StrDup("stk_low_posix") ;

	// file API
	fsi->fsi_init       = stk_low_posix_init ;
        fsi->fsi_destroy    = stk_low_posix_destroy ;
        fsi->fsi_open       = stk_low_posix_open ;
        fsi->fsi_creat      = stk_low_posix_creat ;
        fsi->fsi_close      = stk_low_posix_close ;
        fsi->fsi_read       = stk_low_posix_read ;
        fsi->fsi_write      = stk_low_posix_write ;
        fsi->fsi_lseek      = stk_low_posix_lseek ;
	// directory API
        fsi->fsi_opendir    = stk_low_posix_opendir ;
        fsi->fsi_closedir   = stk_low_posix_closedir ;
        fsi->fsi_readdir    = stk_low_posix_readdir ;
        fsi->fsi_mkdir      = stk_low_posix_mkdir ;
        fsi->fsi_rmdir      = stk_low_posix_rmdir ;
        fsi->fsi_rewinddir  = stk_low_posix_rewinddir ;
	// register/unregister API
        fsi->fsi_register   = stk_low_posix_register ;
        fsi->fsi_unregister = stk_low_posix_unregister ;

	/* return ok */
	return (1) ;
      }

      int stk_low_posix_unregister ( xpni_fsi_t *fsi )
      {
	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* unregister stk_low_posix interface */
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

	/* return ok */
	return (1) ;
      }


      //
      // init + destroy
      //

      int stk_low_posix_init ()
      {
	  return 1 ;
      }

      int stk_low_posix_destroy ()
      {
	  return 1 ;
      }


      //
      // open + close + sread + swrite
      //

      int stk_low_posix_creat ( char *path, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	      return -1 ;
	  }

          /* create file */
          return creat(path, mode) ;
      }

      int stk_low_posix_open ( char *path, int flags, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	     return -1 ;
	  }

          /* open file */
          return open(path, flags, mode) ;
      }

      int stk_low_posix_close ( int fd )
      {
	  return close(fd) ;
      }

      off_t  stk_low_posix_lseek ( int fd, off_t offset, int flag )
      {
	  return lseek(fd, offset, flag) ;
      }

      ssize_t stk_low_posix_write ( int fd, void *buffer, size_t size )
      {
	  return write(fd, buffer, size) ;
      }

      ssize_t stk_low_posix_read ( int fd, void *buffer, size_t size )
      {
	  return read(fd, buffer, size) ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * stk_low_posix_opendir ( char *path )
      {
	  return opendir(path) ;
      }

      int     stk_low_posix_closedir  ( DIR *dirp )
      {
	  return closedir(dirp) ;
      }

      struct dirent *stk_low_posix_readdir ( DIR *dirp )
      {
	  return readdir(dirp) ;
      }

      void    stk_low_posix_rewinddir ( DIR *dirp )
      {
	  return rewinddir(dirp) ;
      }


      long    stk_low_posix_mkdir     ( const char *pathname, int mode )
      {
	  return mkdir(pathname, mode) ;
      }

      long    stk_low_posix_rmdir     ( const char *pathname )
      {
	  return rmdir(pathname) ;
      }


   /* ................................................................... */

