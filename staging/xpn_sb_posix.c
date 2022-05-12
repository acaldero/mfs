

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

	fsi->fsi_init       = xpnsb_posix_init ;
        fsi->fsi_destroy    = xpnsb_posix_destroy ;
        fsi->fsi_open       = xpnsb_posix_open ;
        fsi->fsi_creat      = xpnsb_posix_creat ;
        fsi->fsi_close      = xpnsb_posix_close ;
        fsi->fsi_sread      = xpnsb_posix_sread ;
        fsi->fsi_swrite     = xpnsb_posix_swrite ;
        fsi->fsi_lseek      = xpnsb_posix_lseek ;
        fsi->fsi_opendir    = xpnsb_posix_opendir ;
        fsi->fsi_register   = xpnsb_posix_register ;
        fsi->fsi_unregister = xpnsb_posix_unregister ;
        fsi->fsi_importFile = xpnsb_posix_importFile ;
        fsi->fsi_exportFile = xpnsb_posix_exportFile ;

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
	free(fsi->fsi_name) ;

	fsi->fsi_init       = NULL ;
        fsi->fsi_destroy    = NULL ;
        fsi->fsi_open       = NULL ;
        fsi->fsi_creat      = NULL ;
        fsi->fsi_close      = NULL ;
        fsi->fsi_sread      = NULL ;
        fsi->fsi_swrite     = NULL ;
        fsi->fsi_lseek      = NULL ;
        fsi->fsi_register   = NULL ;
        fsi->fsi_unregister = NULL ;
        fsi->fsi_importFile = NULL ;
        fsi->fsi_exportFile = NULL ;

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

      ssize_t xpnsb_posix_swrite ( int fd, void *buffer, size_t size, off_t offset )
      {
	  lseek(fd, offset, SEEK_SET) ;
	  return write(fd, buffer, size) ;
      }

      ssize_t xpnsb_posix_sread ( int fd, void *buffer, size_t size, off_t offset )
      {
	  lseek(fd, offset, SEEK_SET) ;
	  return read(fd, buffer, size) ;
      }


      //
      // import + export
      //

      int xpnsb_posix_exportFile ( __attribute__((__unused__)) int fd )
      {
	  /* return ok, nothing to do by now... */
	  return (1) ;
      }

      int xpnsb_posix_importFile ( __attribute__((__unused__)) int fd )
      {
	  /* return ok, nothing to do by now... */
	  return (1) ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * xpnsb_posix_opendir ( char *path )
      {
	  return opendir(path) ;
      }


   /* ................................................................... */

