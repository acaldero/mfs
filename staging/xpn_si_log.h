
#ifndef _XPN_SI_LOG_H
#define _XPN_SI_LOG_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "base/time_misc.h"
      #include "xpni/xpni_log_elog.h"
      #include "xpni/xpni_fit.h"
      #include "xpni/xpni_fsit.h"
      #include "xpni/xpni_file.h"


   /* ... Functions / Funciones ......................................... */

      // register + unregister
      int  xpnsi_log_register   ( xpni_fsi_t *fsi ) ;
      int  xpnsi_log_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     xpnsi_log_init    () ;
      int     xpnsi_log_destroy () ;

      // File API
      int     xpnsi_log_creat  ( char *path, mode_t mode ) ;
      int     xpnsi_log_open   ( char *path, int flags, mode_t mode ) ;
      int     xpnsi_log_close  ( int fd ) ;
      off_t   xpnsi_log_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t xpnsi_log_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t xpnsi_log_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    xpnsi_log_mkdir     ( const char *pathname, int mode ) ;
      long    xpnsi_log_rmdir     ( const char *pathname ) ;

      DIR   * xpnsi_log_opendir   ( char *path ) ;
      int     xpnsi_log_closedir  ( DIR *dirp ) ;
      struct dirent *xpnsi_log_readdir ( DIR *dirp ) ;
      void    xpnsi_log_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

