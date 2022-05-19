
#ifndef _STK_MID_LOG_H
#define _STK_MID_LOG_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "base/time_misc.h"
      #include "include/util_log_api.h"
      #include "include/stk_fd.h"
      #include "include/stk_fs.h"


   /* ... Functions / Funciones ......................................... */

      // register + unregister
      int     stk_mid_log_register   ( stk_fs_t *fsi ) ;
      int     stk_mid_log_unregister ( stk_fs_t *fsi ) ;

      // init + destroy
      int     stk_mid_log_init    ( void ) ;
      int     stk_mid_log_destroy ( void ) ;

      // File API
      int     stk_mid_log_creat  ( char *path, mode_t mode ) ;
      int     stk_mid_log_open   ( char *path, int flags, mode_t mode ) ;
      int     stk_mid_log_close  ( int fd ) ;
      off_t   stk_mid_log_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t stk_mid_log_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t stk_mid_log_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    stk_mid_log_mkdir     ( const char *pathname, int mode ) ;
      long    stk_mid_log_rmdir     ( const char *pathname ) ;

      DIR   * stk_mid_log_opendir   ( char *path ) ;
      int     stk_mid_log_closedir  ( DIR *dirp ) ;
      struct dirent *stk_mid_log_readdir ( DIR *dirp ) ;
      void    stk_mid_log_rewinddir ( DIR *dirp ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

