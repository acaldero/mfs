
#ifndef _XPN_SB_XPN_H
#define _XPN_SB_XPN_H

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
      int  xpnsb_xpn_register   ( xpni_fsi_t *fsi ) ;
      int  xpnsb_xpn_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     xpnsb_xpn_init    () ;
      int     xpnsb_xpn_destroy () ;

      // File API
      int     xpnsb_xpn_creat  ( char *path, mode_t mode ) ;
      int     xpnsb_xpn_open   ( char *path, int flags, mode_t mode ) ;
      int     xpnsb_xpn_close  ( int fd ) ;
      off_t   xpnsb_xpn_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t xpnsb_xpn_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t xpnsb_xpn_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    xpnsb_xpn_mkdir     ( const char *pathname, int mode ) ;
      long    xpnsb_xpn_rmdir     ( const char *pathname ) ;

      DIR   * xpnsb_xpn_opendir   ( char *path ) ;
      int     xpnsb_xpn_closedir  ( DIR *dirp ) ;
      struct dirent *xpnsb_xpn_readdir ( DIR *dirp ) ;
      void    xpnsb_xpn_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

