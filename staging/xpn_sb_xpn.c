

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

	fsi->fsi_init       = xpnsb_xpn_init ;
        fsi->fsi_destroy    = xpnsb_xpn_destroy ;
        fsi->fsi_open       = xpnsb_xpn_open ;
        fsi->fsi_creat      = xpnsb_xpn_creat ;
        fsi->fsi_close      = xpnsb_xpn_close ;
        fsi->fsi_sread      = xpnsb_xpn_sread ;
        fsi->fsi_swrite     = xpnsb_xpn_swrite ;
        fsi->fsi_lseek      = xpnsb_xpn_lseek ;
        fsi->fsi_opendir    = xpnsb_xpn_opendir ;
        fsi->fsi_register   = xpnsb_xpn_register ;
        fsi->fsi_unregister = xpnsb_xpn_unregister ;
        fsi->fsi_importFile = xpnsb_xpn_importFile ;
        fsi->fsi_exportFile = xpnsb_xpn_exportFile ;

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

      ssize_t xpnsb_xpn_swrite ( int fd, void *buffer, size_t size, off_t offset )
      {
	  return xpn_pwrite(fd, offset, flag) ;
      }

      ssize_t xpnsb_xpn_sread ( int fd, void *buffer, size_t size, off_t offset )
      {
	  return xpn_pread(fd, offset, flag) ;
      }


      //
      // import + export
      //

      int xpnsb_xpn_exportFile ( __attribute__((__unused__)) int fd )
      {
	  /* return ok, nothing to do by now... */
	  return (1) ;
      }

      int xpnsb_xpn_importFile ( __attribute__((__unused__)) int fd )
      {
	  /* return ok, nothing to do by now... */
	  return (1) ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * xpnsb_xpn_opendir ( char *path )
      {
	  return xpn_opendir(path) ;
      }


   /* ................................................................... */

