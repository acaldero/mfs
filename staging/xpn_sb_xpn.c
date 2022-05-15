

   /* ... Include / Inclusion ........................................... */

      #include "include/xpn_sb_xpn.h"


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int xpnsb_xpn_register   ( xpni_fsi_t *fsi )
      {
	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* register xpnsb_xpn interface */
	fsi->fsi_name   = STRING_MISC_StrDup("xpnsb_xpn") ;

	// file API
	fsi->fsi_init       = xpnsb_xpn_init ;
        fsi->fsi_destroy    = xpnsb_xpn_destroy ;
        fsi->fsi_open       = xpnsb_xpn_open ;
        fsi->fsi_creat      = xpnsb_xpn_creat ;
        fsi->fsi_close      = xpnsb_xpn_close ;
        fsi->fsi_read       = xpnsb_xpn_read ;
        fsi->fsi_write      = xpnsb_xpn_write ;
        fsi->fsi_lseek      = xpnsb_xpn_lseek ;
	// directory API
        fsi->fsi_opendir    = xpnsb_xpn_opendir ;
        fsi->fsi_closedir   = xpnsb_xpn_closedir ;
        fsi->fsi_readdir    = xpnsb_xpn_readdir ;
        fsi->fsi_mkdir      = xpnsb_xpn_mkdir ;
        fsi->fsi_rmdir      = xpnsb_xpn_rmdir ;
        fsi->fsi_rewinddir  = xpnsb_xpn_rewinddir ;
	// register/unregister API
        fsi->fsi_register   = xpnsb_xpn_register ;
        fsi->fsi_unregister = xpnsb_xpn_unregister ;

	/* return ok */
	return (1) ;
      }

      int xpnsb_xpn_unregister ( xpni_fsi_t *fsi )
      {
	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* unregister xpnsb_xpn interface */
	free(fsi->fsi_name) ;
	fsi->fsi_name = NULL ;

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

      int xpnsb_xpn_init ()
      {
	  return xpn_init() ;
      }

      int xpnsb_xpn_destroy ()
      {
	  return xpn_destroy() ;
      }


      //
      // open + close + sread + swrite
      //

      int xpnsb_xpn_creat ( char *path, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	      return -1 ;
	  }

          /* create file */
          return xpn_creat(path, mode) ;
      }

      int xpnsb_xpn_open ( char *path, int flags, mode_t mode )
      {
          int ret ;

          /* check params */
          if (NULL == path) {
	     return -1 ;
	  }

          /* open file */
          return xpn_open(path, flags, mode) ;
      }

      int xpnsb_xpn_close ( int fd )
      {
	  return xpn_close(fd) ;
      }

      off_t  xpnsb_xpn_lseek ( int fd, off_t offset, int flag )
      {
	  return xpn_lseek(fd, offset, flag) ;
      }

      ssize_t xpnsb_xpn_write ( int fd, void *buffer, size_t size )
      {
	  return xpn_pwrite(fd, offset, flag) ; // TODO
      }

      ssize_t xpnsb_xpn_read ( int fd, void *buffer, size_t size )
      {
	  return xpn_pread(fd, offset, flag) ; // TODO
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * xpnsb_xpn_opendir ( char *path )
      {
	  return xpn_opendir(path) ;
      }

      int     xpnsb_xpn_closedir  ( DIR *dirp )
      {
	  return xpn_closedir(dirp) ;
      }

      struct dirent *xpnsb_xpn_readdir ( DIR *dirp )
      {
	  return xpn_readdir(dirp) ;
      }

      void    xpnsb_xpn_rewinddir ( DIR *dirp )
      {
	  return xpn_rewinddir(dirp) ;
      }


      long    xpnsb_xpn_mkdir     ( const char *pathname, int mode )
      {
	  return xpn_mkdir(pathname, mode) ;
      }

      long    xpnsb_xpn_rmdir     ( const char *pathname )
      {
	  return xpn_rmdir(pathname) ;
      }


   /* ................................................................... */

