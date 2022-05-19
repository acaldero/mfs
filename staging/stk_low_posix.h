
#ifndef _STK_LOW_POSIX_H
#define _STK_LOW_POSIX_H

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
      int     stk_low_posix_register   ( xpni_fsi_t *fsi ) ;
      int     stk_low_posix_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     stk_low_posix_init    () ;
      int     stk_low_posix_destroy () ;

      // File API
      int     stk_low_posix_creat  ( char *path, mode_t mode ) ;
      int     stk_low_posix_open   ( char *path, int flags, mode_t mode ) ;
      int     stk_low_posix_close  ( int fd ) ;
      off_t   stk_low_posix_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t stk_low_posix_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t stk_low_posix_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    stk_low_posix_mkdir     ( const char *pathname, int mode ) ;
      long    stk_low_posix_rmdir     ( const char *pathname ) ;

      DIR   * stk_low_posix_opendir   ( char *path ) ;
      int     stk_low_posix_closedir  ( DIR *dirp ) ;
      struct dirent *stk_low_posix_readdir ( DIR *dirp ) ;
      void    stk_low_posix_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

