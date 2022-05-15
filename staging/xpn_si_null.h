
#ifndef _XPN_SI_NULL_H
#define _XPN_SI_NULL_H

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
      int  xpnsi_null_register   ( xpni_fsi_t *fsi ) ;
      int  xpnsi_null_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     xpnsi_null_init    () ;
      int     xpnsi_null_destroy () ;

      // File API
      int     xpnsi_null_creat  ( char *path, mode_t mode ) ;
      int     xpnsi_null_open   ( char *path, int flags, mode_t mode ) ;
      int     xpnsi_null_close  ( int fd ) ;
      off_t   xpnsi_null_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t xpnsi_null_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t xpnsi_null_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    xpnsi_null_mkdir     ( const char *pathname, int mode ) ;
      long    xpnsi_null_rmdir     ( const char *pathname ) ;

      DIR   * xpnsi_null_opendir   ( char *path ) ;
      int     xpnsi_null_closedir  ( DIR *dirp ) ;
      struct dirent *xpnsi_null_readdir ( DIR *dirp ) ;
      void    xpnsi_null_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

