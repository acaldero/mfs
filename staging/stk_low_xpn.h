
#ifndef _STK_LOW_XPN_H
#define _STK_LOW_XPN_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "xpni/xpni_fit.h"
      #include "xpni/xpni_fsit.h"
      #include "xpni/xpni_file.h"


   /* ... Functions / Funciones ......................................... */

      // register + unregister
      int     stk_low_xpn_register   ( xpni_fsi_t *fsi ) ;
      int     stk_low_xpn_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     stk_low_xpn_init    () ;
      int     stk_low_xpn_destroy () ;

      // File API
      int     stk_low_xpn_creat  ( char *path, mode_t mode ) ;
      int     stk_low_xpn_open   ( char *path, int flags, mode_t mode ) ;
      int     stk_low_xpn_close  ( int fd ) ;
      off_t   stk_low_xpn_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t stk_low_xpn_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t stk_low_xpn_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    stk_low_xpn_mkdir     ( const char *pathname, int mode ) ;
      long    stk_low_xpn_rmdir     ( const char *pathname ) ;

      DIR   * stk_low_xpn_opendir   ( char *path ) ;
      int     stk_low_xpn_closedir  ( DIR *dirp ) ;
      struct dirent *stk_low_xpn_readdir ( DIR *dirp ) ;
      void    stk_low_xpn_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

