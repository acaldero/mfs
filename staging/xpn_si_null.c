

   /* ... Include / Inclusion ........................................... */

      #include "include/xpn_si_null.h"


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int xpni_null_register   ( xpni_fsi_t *fsi )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%p", fsi) ;

	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* register xpni_null interface */
	fsi->fsi_name   = STRING_MISC_StrDup("xpni_null") ;

	fsi->fsi_init       = xpni_null_init ;
        fsi->fsi_destroy    = xpni_null_destroy ;
        fsi->fsi_open       = xpni_null_open ;
        fsi->fsi_creat      = xpni_null_creat ;
        fsi->fsi_close      = xpni_null_close ;
        fsi->fsi_sread      = xpni_null_sread ;
        fsi->fsi_swrite     = xpni_null_swrite ;
        fsi->fsi_lseek      = xpni_null_lseek ;
        fsi->fsi_opendir    = xpni_null_opendir ;
        fsi->fsi_register   = xpni_null_register ;
        fsi->fsi_unregister = xpni_null_unregister ;
        fsi->fsi_importFile = xpni_null_importFile ;
        fsi->fsi_exportFile = xpni_null_exportFile ;

	XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	/* return ok */
	return (1) ;
      }

      int xpni_null_unregister ( xpni_fsi_t *fsi )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%p", fsi) ;

	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* unregister xpni_null interface */
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

	XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	/* return ok */
	return (1) ;
      }


      //
      // init + destroy
      //

      int xpni_null_init ()
      {
	int ret ;

	XPN_DEBUG_BEGIN();
	ret = xpni_lowfsi_init();
	XPN_DEBUG_END();

	return ret ;
      }

      int xpni_null_destroy ()
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

      int xpni_null_creat ( int fd )
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

      int xpni_null_open ( int fd )
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

      int xpni_null_close ( int fd )
      {
	int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;
	ret = xpni_lowfsi_close(xpni_fit_get_XPN_DATA_FD(fd));
	XPN_DEBUG_END_CUSTOM("%d", fd) ;

	return ret ;
      }

      off_t  xpni_null_lseek ( int fd, off_t offset, int flag )
      {
	int ret ;

        /* lseek file: data only */
	XPN_DEBUG_BEGIN_CUSTOM("%d %d %d", fd, offset, flag) ;
	ret = xpni_lowfsi_lseek(xpni_fit_get_XPN_DATA_FD(fd),offset,flag);
	XPN_DEBUG_END_CUSTOM("%d %d %d", fd, offset, flag) ;

	return ret ;
      }

      ssize_t xpni_null_swrite
      (
         int fd,
         void *buffer,
         off_t offset,
         size_t size
      )
      {
	int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;
	ret = xpni_lowfsi_pwrite(xpni_fit_get_XPN_DATA_FD(fd), buffer, offset, size);
	XPN_DEBUG_END_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;

	return ret ;
      }

      ssize_t xpni_null_sread
      (
         int fd,
         void *buffer,
         off_t offset,
         size_t size
      )
      {
	int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;
	ret = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd), buffer, offset, size);
	XPN_DEBUG_END_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;

	return ret ;
      }


      //
      // import + export
      //

      int xpni_null_exportFile ( __attribute__((__unused__)) int fd )
      {
	int ret ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;
	XPN_DEBUG_END_CUSTOM("%d", fd) ;

	/* return ok, nothing to do by now... */
	return (1) ;
      }

      int xpni_null_importFile ( __attribute__((__unused__)) int fd )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;
	XPN_DEBUG_END_CUSTOM("%d", fd) ;

	/* return ok, nothing to do by now... */
	return (1) ;

      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * xpni_null_opendir ( int dd )
      {
        DIR  *ret;

	XPN_DEBUG_BEGIN_CUSTOM("%d", dd) ;

        /* open directory */
	ret = xpni_lowfsi_opendir(xpni_fit_get_XPN_FNAME(dd));

        /* update directory description */
        xpni_fit_set_XPN_DATA_FD(dd,(long)ret) ; /* sizeof(int) === sizeof(DIR *) */

	XPN_DEBUG_END_CUSTOM("%d", dd) ;

	return ret;
      }


   /* ................................................................... */

