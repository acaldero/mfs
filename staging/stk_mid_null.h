
#ifndef _STK_MID_NULL_H
#define _STK_MID_NULL_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "base/time_misc.h"
      #include "xpni/util_log_api.h"
      #include "xpni/xpni_fit.h"
      #include "xpni/xpni_fsit.h"
      #include "xpni/xpni_file.h"


   /* ... Functions / Funciones ......................................... */

      // register + unregister
      int     stk_mid_null_register   ( xpni_fsi_t *fsi ) ;
      int     stk_mid_null_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     stk_mid_null_init    () ;
      int     stk_mid_null_destroy () ;

      // File API
      int     stk_mid_null_creat  ( char *path, mode_t mode ) ;
      int     stk_mid_null_open   ( char *path, int flags, mode_t mode ) ;
      int     stk_mid_null_close  ( int fd ) ;
      off_t   stk_mid_null_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t stk_mid_null_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t stk_mid_null_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    stk_mid_null_mkdir     ( const char *pathname, int mode ) ;
      long    stk_mid_null_rmdir     ( const char *pathname ) ;

      DIR   * stk_mid_null_opendir   ( char *path ) ;
      int     stk_mid_null_closedir  ( DIR *dirp ) ;
      struct dirent *stk_mid_null_readdir ( DIR *dirp ) ;
      void    stk_mid_null_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

