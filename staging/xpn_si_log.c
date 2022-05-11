

   /* ... Include / Inclusion ........................................... */

      #include "include/xpn_si_log.h"


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int xpni_log_register   ( xpni_fsi_t *fsi )
      {
        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpni_log_register(%p); \n",
                   __FILE__,__LINE__,fsi);
	#endif

	/* check params */
	if (NULL == fsi)
		return (-1) ;

	/* register xpni_log interface */
	fsi->fsi_name   = STRING_MISC_StrDup("xpni_log") ;

	fsi->fsi_init       = xpni_log_init ;
        fsi->fsi_destroy    = xpni_log_destroy ;
        fsi->fsi_open       = xpni_log_open ;
        fsi->fsi_creat      = xpni_log_creat ;
        fsi->fsi_close      = xpni_log_close ;
        fsi->fsi_sread      = xpni_log_sread ;
        fsi->fsi_swrite     = xpni_log_swrite ;
        fsi->fsi_lseek      = xpni_log_lseek ;
        fsi->fsi_opendir    = xpni_log_opendir ;
        fsi->fsi_importFile = xpni_log_importFile ;
        fsi->fsi_exportFile = xpni_log_exportFile ;
        fsi->fsi_register   = xpni_log_register ;
        fsi->fsi_unregister = xpni_log_unregister ;


	/* return ok */
	return (1) ;
      }

      int xpni_log_unregister ( xpni_fsi_t *fsi )
      {
        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpni_log_unregister(%p); \n",
                   __FILE__,__LINE__,fsi);
	#endif

	/* check params */
	if (NULL == fsi)
		return (-1) ;

	/* unregister xpni_log interface */
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

      int xpni_log_init ()
      {
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN();

	/* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_init();
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_init",-1,-1,-1);

	XPN_DEBUG_END();

        /* return 'ret' */
	return ret ;
      }

      int xpni_log_destroy ()
      {
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN();

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_destroy();
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_destroy",-1,-1,-1);

	XPN_DEBUG_END();

        /* return 'ret' */
	return ret ;
      }



      //
      // open + close + sread + swrite
      //

      int xpni_log_creat ( int fd )
      {
        int             ret ;
	struct timeval  t1, t2 ;
        xpni_fi_t *xpni_fi1 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

        /* check params */
        xpni_fi1 = xpni_fit_get(fd) ;
        if (NULL == xpni_fi1)
	    return (-1) ;

	/* passthru... */
	TIME_MISC_Timer(&t1);
        ret = xpni_lowfsi_creat(xpni_fit_get_XPN_FNAME(fd),
                                xpni_fit_get_XPN_MODE(fd)) ;
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpn_creat",fd,-1,-1);

        /* update file description */
        if (ret >= 0) {
            xpni_fit_set_XPN_DATA_FD(fd,ret) ;
	}

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return xpn file descriptor */
        return ret ;
      }

      int xpni_log_open ( int fd )
      {
	int             ret ;
	struct timeval  t1, t2 ;
        xpni_fi_t      *xpni_fi1 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

        /* check params */
        xpni_fi1 = xpni_fit_get(fd) ;
        if (NULL == xpni_fi1) {
	    return (-1) ;
	}

	/* passthru... */
	TIME_MISC_Timer(&t1);
        ret = xpni_lowfsi_open(xpni_fit_get_XPN_FNAME(fd),
                               xpni_fit_get_XPN_FLAG(fd),
                               xpni_fit_get_XPN_MODE(fd)) ;
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_open",fd,-1,-1);

        /* update file description */
        if (ret >= 0) {
            xpni_fit_set_XPN_DATA_FD(fd,ret) ;
	}

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return xpn file descriptor */
        return ret ;
      }

      int xpni_log_close ( int fd )
      {
	int    data_fd ;
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

        /* passthru... */
	data_fd = xpni_fit_get_XPN_DATA_FD(fd);
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_close(data_fd);
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_close",fd,-1,-1);

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return 'ret' */
	return ret ;
      }

      off_t  xpni_log_lseek ( int fd, off_t offset, int flag )
      {
	int    data_fd ;
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %d %d", fd, offset, flag) ;

	/* passthru... */
	data_fd = xpni_fit_get_XPN_DATA_FD(fd);
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_lseek(data_fd,offset,flag);
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_lseek",fd,-1,-1);

	XPN_DEBUG_END_CUSTOM("%d %d %d", fd, offset, flag) ;

        /* return 'ret' */
	return ret ;
      }

      ssize_t xpni_log_sread
      (
         int fd,
         void *buffer,
         off_t offset,
         size_t size
      )
      {
	int             data_fd ;
	int             ret ;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;

        /* passthru... */
	data_fd = xpni_fit_get_XPN_DATA_FD(fd);
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_pread(data_fd,
                                buffer,
                                offset,
                                size);
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_sread",fd,offset,size);

	XPN_DEBUG_END_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;

        /* return 'ret' */
	return ret;
      }

      ssize_t xpni_log_swrite
      (
         int     fd,
         void   *buffer,
         off_t   offset,
         size_t  size
      )
      {
	int             data_fd ;
	int             ret ;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;

        /* passthru... */
	data_fd = xpni_fit_get_XPN_DATA_FD(fd);
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_pwrite(data_fd,
                                 buffer,
                                 offset,
                                 size);
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_swrite",fd,offset,size);

	XPN_DEBUG_END_CUSTOM("%d %p %ld %d", fd, buffer, (long)offset, size) ;

        /* return 'ret' */
	return ret;
      }


      //
      // import + export
      //

      int xpni_log_exportFile ( int fd )
      {
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

	/* nothing to do, by now... */
	TIME_MISC_Timer(&t1);
	ret = 1;
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpn_ioctl.exportFile!",fd,-1,-1);

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return 'ret' */
	return ret ;
      }

      int xpni_log_importFile ( int fd )
      {
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

	/* nothing to do, by now... */
	TIME_MISC_Timer(&t1);
	ret = 1;
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpn_ioctl.importFile!",fd,-1,-1);

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return 'ret' */
	return ret ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * xpni_log_opendir ( int dd )
      {
        DIR  *ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", dd) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_opendir(xpni_fit_get_XPN_FNAME(dd));
	TIME_MISC_Timer(&t2);

        /* record event */
	xpni_log_elog(&t1,&t2,"xpni_lowfsi_opendir",dd,-1,-1);

        /* update directory description */
        if ((long)ret >= 0) {
            xpni_fit_set_XPN_DATA_FD(dd, (long)ret) ; /* sizeof(int) === sizeof(DIR *) */
	}

	XPN_DEBUG_END_CUSTOM("%d", dd) ;

        /* return xpn directory descriptor */
	return ret;
      }


   /* ................................................................... */

