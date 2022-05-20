
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

      #include "include/stk_mid_log.h"


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int stk_mid_log_register   ( stk_fs_t *fsi )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%p", fsi) ;

	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* register stk_mid_log interface */
	fsi->fsi_name   = STRING_MISC_StrDup("stk_mid_log") ;

	// file API
	fsi->fsi_init       = stk_mid_log_init ;
        fsi->fsi_finalize   = stk_mid_log_finalize ;
        fsi->fsi_open       = stk_mid_log_open ;
        fsi->fsi_creat      = stk_mid_log_creat ;
        fsi->fsi_close      = stk_mid_log_close ;
        fsi->fsi_read       = stk_mid_log_read ;
        fsi->fsi_write      = stk_mid_log_write ;
        fsi->fsi_lseek      = stk_mid_log_lseek ;
	// directory API
        fsi->fsi_opendir    = stk_mid_log_opendir ;
        fsi->fsi_closedir   = stk_mid_log_closedir ;
        fsi->fsi_readdir    = stk_mid_log_readdir ;
        fsi->fsi_mkdir      = stk_mid_log_mkdir ;
        fsi->fsi_rmdir      = stk_mid_log_rmdir ;
        fsi->fsi_rewinddir  = stk_mid_log_rewinddir ;
	// register/unregister API
        fsi->fsi_register   = stk_mid_log_register ;
        fsi->fsi_unregister = stk_mid_log_unregister ;

	XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	/* return ok */
	return (1) ;
      }

      int stk_mid_log_unregister ( stk_fs_t *fsi )
      {
	XPN_DEBUG_BEGIN_CUSTOM("%p", fsi) ;

	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* unregister stk_mid_log interface */
	if (NULL != fsi->fsi_name) {
	    free(fsi->fsi_name) ;
	    fsi->fsi_name   = NULL ;
	}

	// file API
	fsi->fsi_init       = NULL ;
        fsi->fsi_finalize   = NULL ;
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
      // init + finalize
      //

      int stk_mid_log_init ( void )
      {
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN();

	/* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_init();
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_init",-1,-1,-1);

	XPN_DEBUG_END();

        /* return 'ret' */
	return ret ;
      }

      int stk_mid_log_finalize ( void )
      {
	int    ret ;
	struct timeval t1, t2 ;

	XPN_DEBUG_BEGIN();

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_finalize();
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_finalize",-1,-1,-1);

	XPN_DEBUG_END();

        /* return 'ret' */
	return ret ;
      }



      //
      // open + close + sread + swrite
      //

      int stk_mid_log_creat ( int fd )
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
	util_log_elog(&t1,&t2,"xpn_creat",fd,-1,-1);

        /* update file description */
        if (ret >= 0) {
            xpni_fit_set_XPN_DATA_FD(fd,ret) ;
	}

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return xpn file descriptor */
        return ret ;
      }

      int stk_mid_log_open ( int fd )
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
	util_log_elog(&t1,&t2,"xpni_lowfsi_open",fd,-1,-1);

        /* update file description */
        if (ret >= 0) {
            xpni_fit_set_XPN_DATA_FD(fd,ret) ;
	}

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return xpn file descriptor */
        return ret ;
      }

      int stk_mid_log_close ( int fd )
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
	util_log_elog(&t1,&t2,"xpni_lowfsi_close",fd,-1,-1);

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        /* return 'ret' */
	return ret ;
      }

      off_t  stk_mid_log_lseek ( int fd, off_t offset, int flag )
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
	util_log_elog(&t1,&t2,"xpni_lowfsi_lseek",fd,-1,-1);

	XPN_DEBUG_END_CUSTOM("%d %d %d", fd, offset, flag) ;

        /* return 'ret' */
	return ret ;
      }

      ssize_t stk_mid_log_read ( int fd, void *buffer, size_t size )
      {
	int             data_fd ;
	int             ret ;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* passthru... */
	data_fd = xpni_fit_get_XPN_DATA_FD(fd);
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_read(data_fd, buffer, size);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_read",fd,size,-1);

	XPN_DEBUG_END_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* return 'ret' */
	return ret;
      }

      ssize_t stk_mid_log_swrite ( int  fd, void *buffer, size_t  size )
      {
	int             data_fd ;
	int             ret ;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* passthru... */
	data_fd = xpni_fit_get_XPN_DATA_FD(fd);
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_write(data_fd, buffer, size);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_write",fd,size,-1);

	XPN_DEBUG_END_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* return 'ret' */
	return ret;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * stk_mid_log_opendir ( int dd )
      {
        DIR  *ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", dd) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_opendir(xpni_fit_get_XPN_FNAME(dd));
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_opendir",dd,-1,-1);

        /* update directory description */
        if ((long)ret >= 0) {
            xpni_fit_set_XPN_DATA_FD(dd, (long)ret) ; /* sizeof(int) === sizeof(DIR *) */
	}

	XPN_DEBUG_END_CUSTOM("%d", dd) ;

        /* return xpn directory descriptor */
	return ret;
      }

      int     stk_mid_log_closedir  ( DIR *dirp )
      {
        int ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_closedir(dirp);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_closedir",dirp,-1,-1);

	XPN_DEBUG_END_CUSTOM("%p", dirp) ;

        /* return xpn directory descriptor */
	return ret;
      }

      struct dirent *stk_mid_log_readdir ( DIR *dirp )
      {
        struct dirent * ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_readdir(dirp);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_readdir",dirp,-1,-1);

	XPN_DEBUG_END_CUSTOM("%p", dirp) ;

        /* return xpn directory descriptor */
	return ret;
      }

      void    stk_mid_log_rewinddir ( DIR *dirp )
      {
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	xpni_lowfsi_rewinddir(dirp);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_rewinddir",dirp,-1,-1);

	XPN_DEBUG_END_CUSTOM("%p", dirp) ;
      }


      long    stk_mid_log_mkdir     ( const char *pathname, int mode )
      {
        long ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%s %d", pathname, mode) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_mkdir(pathname, mode);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_mkdir",pathname,mode,-1);

	XPN_DEBUG_END_CUSTOM("%s %d", pathname, mode) ;

        /* return xpn directory descriptor */
	return ret;
      }

      long    stk_mid_log_rmdir     ( const char *pathname )
      {
        long ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%s", pathname) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = xpni_lowfsi_rmdir(pathname);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"xpni_lowfsi_rmdir",pathname,mode,-1);

	XPN_DEBUG_END_CUSTOM("%s", pathname) ;

        /* return xpn directory descriptor */
	return ret;
      }


   /* ................................................................... */

