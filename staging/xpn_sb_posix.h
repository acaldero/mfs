
#ifndef _XPN_SB_POSIX_H
#define _XPN_SB_POSIX_H

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
      int  xpnsb_posix_register   ( xpni_fsi_t *fsi ) ;
      int  xpnsb_posix_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     xpnsb_posix_init    () ;
      int     xpnsb_posix_destroy () ;

      // File API
      int     xpnsb_posix_creat  ( char *path, mode_t mode ) ;
      int     xpnsb_posix_open   ( char *path, int flags, mode_t mode ) ;
      int     xpnsb_posix_close  ( int fd ) ;
      off_t   xpnsb_posix_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t xpnsb_posix_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t xpnsb_posix_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    xpnsb_posix_mkdir     ( const char *pathname, int mode ) ;
      long    xpnsb_posix_rmdir     ( const char *pathname ) ;

      DIR   * xpnsb_posix_opendir   ( char *path ) ;
      int     xpnsb_posix_closedir  ( DIR *dirp ) ;
      struct dirent *xpnsb_posix_readdir ( DIR *dirp ) ;
      void    xpnsb_posix_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

