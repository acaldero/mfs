
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
      // Init + finalize
      //

      int stk_mid_log_init ( stk_fs_t *fsi, stk_fs_t *low_fsi )
      {
	  int ret ;
	  struct timeval t1, t2 ;

	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p, low_fsi:%p", fsi, low_fsi) ;

	  /* check params */
	  if (NULL == fsi) {
	      return -1 ;
	  }

	  /* register stk_mid_log interface */
	  fsi->fsi_name       = STRING_MISC_StrDup("stk_mid_log") ;
	  fsi->low_fsi        = low_fsi ;

	  // init/finalize
	  fsi->fsi_init       = stk_mid_log_init ;
          fsi->fsi_finalize   = stk_mid_log_finalize ;
	  // file API
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

	  // TODO: check if stack builder should do this instead
	  TIME_MISC_Timer(&t1);
	  ret = STK_FS_INIT(fsi->low_fsi, NULL) ;
	  TIME_MISC_Timer(&t2);

          /* record event */
	  util_log_elog(&t1,&t2,"lowfsi_init",-1,-1,-1);

	  XPN_DEBUG_END_CUSTOM("fsi:%p, low_fsi:%p -> %d", fsi, low_fsi, ret) ;

	  /* return ok */
	  return (1) ;
      }

      int stk_mid_log_finalize ( stk_fs_t *fsi )
      {
	  int       ret ;
          stk_fs_t *low_fsi ;
	  struct timeval t1, t2 ;

	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p", fsi) ;

	  // TODO: check if stack builder should do this instead
	  TIME_MISC_Timer(&t1);
          low_fsi = STK_FS_GET_LOWFSI(fsi) ;
	  if (NULL != low_fsi) {
              ret = STK_FS_FINALIZE(low_fsi) ;
	  }
	  TIME_MISC_Timer(&t2);

          /* record event */
	  util_log_elog(&t1,&t2,"lowfsi_finalize",-1,-1,-1);

	  // finalize
          ret = -1 ;
	  if (NULL != fsi) {
              ret = stk_fs_finalize(fsi) ;
	  }

	  XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	  /* return ok|ko */
	  return ret ;
      }


      //
      // open + close + sread + swrite
      //

      int stk_mid_log_creat ( stk_fs_t *fsi, char *path, mode_t mode )
      {
          int             ret ;
  	  struct timeval  t1, t2 ;

	  XPN_DEBUG_BEGIN_CUSTOM("path:%s, mode:%d", path, (int)mode) ;

          /* create file */
	  ret = -1 ;
          if (NULL != path)
	  {
	      /* passthru... */
	      TIME_MISC_Timer(&t1);
              ret = STK_FS_CREAT(path, mode) ;
	      TIME_MISC_Timer(&t2);

              /* record event */
	      util_log_elog(&t1,&t2,"STK_FS_CREAT",fd,-1,-1);
	  }

	  XPN_DEBUG_END_CUSTOM("path:%s, mode:%d -> %d", path, (int)mode, ret) ;

          /* return xpn file descriptor */
          return ret ;
      }

      int stk_mid_log_open ( stk_fs_t *fsi, char *path, int flags, mode_t mode )
      {
	  int             ret ;
	  struct timeval  t1, t2 ;

	  XPN_DEBUG_BEGIN_CUSTOM("path:%s, flags:%d, mode:%d", path, flags, (int)mode) ;

          /* open file */
	  ret = -1 ;
          if (NULL != path)
	  {
	      /* passthru... */
	      TIME_MISC_Timer(&t1);
              ret = STK_FS_OPEN(path, flags, mode) ;
	      TIME_MISC_Timer(&t2);

              /* record event */
	      util_log_elog(&t1,&t2,"STK_FS_OPEN",fd,-1,-1);
	  }

	  XPN_DEBUG_END_CUSTOM("path:%s, flags:%d, mode:%d", path, flags, (int)mode) ;

          /* return xpn file descriptor */
          return ret ;
      }

      int stk_mid_log_close ( stk_fs_t *fsi, int fd )
      {
	  int    ret ;
	  struct timeval t1, t2 ;

	  XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

          /* passthru... */
	  TIME_MISC_Timer(&t1);
	  ret = STK_FS_CLOSE(fd);
	  TIME_MISC_Timer(&t2);

          /* record event */
	  util_log_elog(&t1,&t2,"STK_FS_CLOSE",fd,-1,-1);

	  XPN_DEBUG_END_CUSTOM("%d", fd) ;

          /* return 'ret' */
	  return ret ;
      }

      off_t  stk_mid_log_lseek ( stk_fs_t *fsi, int fd, off_t offset, int flag )
      {
	  int    ret ;
	  struct timeval t1, t2 ;

	  XPN_DEBUG_BEGIN_CUSTOM("%d %d %d", fd, offset, flag) ;

	  /* passthru... */
	  TIME_MISC_Timer(&t1);
	  ret = STK_FS_LSEEK(fd,offset,flag);
	  TIME_MISC_Timer(&t2);

          /* record event */
	  util_log_elog(&t1,&t2,"STK_FS_LSEEK",fd,-1,-1);

	  XPN_DEBUG_END_CUSTOM("%d %d %d", fd, offset, flag) ;

          /* return 'ret' */
	  return ret ;
      }

      ssize_t stk_mid_log_write ( stk_fs_t *fsi, int fd, void *buffer, size_t size )
      {
	int             ret ;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = STK_FS_WRITE(fd, buffer, size);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_WRITE",fd,size,-1);

	XPN_DEBUG_END_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* return 'ret' */
	return ret;
      }

      ssize_t stk_mid_log_read ( stk_fs_t *fsi, int fd, void *buffer, size_t size )
      {
	int             ret ;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = STK_FS_READ(fd, buffer, size);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_READ",fd,size,-1);

	XPN_DEBUG_END_CUSTOM("%d %p %d", fd, buffer, size) ;

        /* return 'ret' */
	return ret;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR * stk_mid_log_opendir ( stk_fs_t *fsi, int dd )
      {
        DIR  *ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", dd) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = STK_FS_OPENDIR(xpni_fit_get_XPN_FNAME(dd));
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_OPENDIR",dd,-1,-1);

        /* update directory description */
        if ((long)ret >= 0) {
            xpni_fit_set_XPN_DATA_FD(dd, (long)ret) ; /* sizeof(int) === sizeof(DIR *) */
	}

	XPN_DEBUG_END_CUSTOM("%d", dd) ;

        /* return xpn directory descriptor */
	return ret;
      }

      int     stk_mid_log_closedir  ( stk_fs_t *fsi, DIR *dirp )
      {
        int ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = STK_FS_CLOSEDIR(dirp);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_CLOSEDIR",dirp,-1,-1);

	XPN_DEBUG_END_CUSTOM("%p", dirp) ;

        /* return xpn directory descriptor */
	return ret;
      }

      struct dirent *stk_mid_log_readdir ( stk_fs_t *fsi, DIR *dirp )
      {
        struct dirent * ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = STK_FS_READDIR(dirp);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_READDIR",dirp,-1,-1);

	XPN_DEBUG_END_CUSTOM("%p", dirp) ;

        /* return xpn directory descriptor */
	return ret;
      }

      void    stk_mid_log_rewinddir ( stk_fs_t *fsi, DIR *dirp )
      {
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	STK_FS_REWINDDIR(dirp);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_REWINDDIR",dirp,-1,-1);

	XPN_DEBUG_END_CUSTOM("%p", dirp) ;
      }


      long    stk_mid_log_mkdir     ( stk_fs_t *fsi, const char *pathname, int mode )
      {
        long ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%s %d", pathname, mode) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = STK_FS_MKDIR(pathname, mode);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_MKDIR",pathname,mode,-1);

	XPN_DEBUG_END_CUSTOM("%s %d", pathname, mode) ;

        /* return xpn directory descriptor */
	return ret;
      }

      long    stk_mid_log_rmdir     ( stk_fs_t *fsi, const char *pathname )
      {
        long ret;
	struct timeval  t1, t2 ;

	XPN_DEBUG_BEGIN_CUSTOM("%s", pathname) ;

        /* passthru... */
	TIME_MISC_Timer(&t1);
	ret = STK_FS_RMDIR(pathname);
	TIME_MISC_Timer(&t2);

        /* record event */
	util_log_elog(&t1,&t2,"STK_FS_RMDIR",pathname,mode,-1);

	XPN_DEBUG_END_CUSTOM("%s", pathname) ;

        /* return xpn directory descriptor */
	return ret;
      }


   /* ................................................................... */

