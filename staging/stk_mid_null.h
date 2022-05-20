
#ifndef _STK_MID_NULL_H
#define _STK_MID_NULL_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "base/time_misc.h"
      #include "xpni/util_log_api.h"
      #include "include/stk_fd.h"
      #include "include/stk_fs.h"


   /* ... Const / Constantes ............................................ */

      #define FS_TAG_NULL        "stk_mid_null"


   /* ... Functions / Funciones ......................................... */

      // register + unregister
      int     stk_mid_null_register   ( stk_fs_t *fsi ) ;
      int     stk_mid_null_unregister ( stk_fs_t *fsi ) ;

      // init + destroy
      int     stk_mid_null_init    ( void ) ;
      int     stk_mid_null_destroy ( void ) ;

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

