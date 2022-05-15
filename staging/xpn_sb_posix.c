

   /* ... Include / Inclusion ........................................... */

      #include "include/xpn_sb_posix.h"


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int xpnsb_posix_register   ( xpni_fsi_t *fsi )
      {
	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* register xpnsb_posix interface */
	fsi->fsi_name   = STRING_MISC_StrDup("xpnsb_posix") ;

	// file API
	fsi->fsi_init       = xpnsb_posix_init ;
        fsi->fsi_destroy    = xpnsb_posix_destroy ;
        fsi->fsi_open       = xpnsb_posix_open ;
        fsi->fsi_creat      = xpnsb_posix_creat ;
        fsi->fsi_close      = xpnsb_posix_close ;
        fsi->fsi_read       = xpnsb_posix_read ;
        fsi->fsi_write      = xpnsb_posix_write ;
        fsi->fsi_lseek      = xpnsb_posix_lseek ;
	// directory API
        fsi->fsi_opendir    = xpnsb_posix_opendir ;
        fsi->fsi_closedir   = xpnsb_posix_closedir ;
        fsi->fsi_readdir    = xpnsb_posix_readdir ;
        fsi->fsi_mkdir      = xpnsb_posix_mkdir ;
        fsi->fsi_rmdir      = xpnsb_posix_rmdir ;
        fsi->fsi_rewinddir  = xpnsb_posix_rewinddir ;
	// register/unregister API
        fsi->fsi_register   = xpnsb_posix_register ;
        fsi->fsi_unregister = xpnsb_posix_unregister ;

	/* return ok */
	return (1) ;
      }

      int xpnsb_posix_unregister ( xpni_fsi_t *fsi )
      {
	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* unregister xpnsb_posix interface */
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

      int xpnsb_posix_init ()
      {
	  return 1 ;
      }

      int xpnsb_posix_destroy ()
      {
	  return 1 ;
      }


      //
      // open + close + sread + swrite
      //

      int xpnsb_posix_creat ( char *path, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	      return -1 ;
	  }

          /* create file */
          return creat(path, mode) ;
      }

      int xpnsb_posix_open ( char *path, int flags, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	     return -1 ;
	  }

          /* open file */
          return open(path, flags, mode) ;
      }

      int xpnsb_posix_close ( int fd )
      {
	  return close(fd) ;
      }

      off_t  xpnsb_posix_lseek ( int fd, off_t offset, int flag )
      {
	  return lseek(fd, offset, flag) ;
      }

      ssize_t xpnsb_posix_write ( int fd, void *buffer, size_t size )
      {
	  return write(fd, buffer, size) ;
      }

      ssize_t xpnsb_posix_read ( int fd, void *buffer, size_t size )
      {
	  return read(fd, buffer, size) ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * xpnsb_posix_opendir ( char *path )
      {
	  return opendir(path) ;
      }

      int     xpnsb_posix_closedir  ( DIR *dirp )
      {
	  return closedir(dirp) ;
      }

      struct dirent *xpnsb_posix_readdir ( DIR *dirp )
      {
	  return readdir(dirp) ;
      }

      void    xpnsb_posix_rewinddir ( DIR *dirp )
      {
	  return rewinddir(dirp) ;
      }


      long    xpnsb_posix_mkdir     ( const char *pathname, int mode )
      {
	  return mkdir(pathname, mode) ;
      }

      long    xpnsb_posix_rmdir     ( const char *pathname )
      {
	  return rmdir(pathname) ;
      }


   /* ................................................................... */

