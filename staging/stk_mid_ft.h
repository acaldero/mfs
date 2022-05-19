
#ifndef _STK_MID_FT_H
#define _STK_MID_FT_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "base/math_misc.h"
      #include "xpni/xpni_fit.h"
      #include "xpni/xpni_fsit.h"
      #include "xpni/xpni_file.h"


   /* ... Functions / Funciones ......................................... */

      // register + unregister
      int     stk_mid_ft_register   ( xpni_fsi_t *fsi ) ;
      int     stk_mid_ft_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     stk_mid_ft_init    () ;
      int     stk_mid_ft_destroy () ;

      // File API
      int     stk_mid_ft_creat  ( char *path, mode_t mode ) ;
      int     stk_mid_ft_open   ( char *path, int flags, mode_t mode ) ;
      int     stk_mid_ft_close  ( int fd ) ;
      off_t   stk_mid_ft_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t stk_mid_ft_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t stk_mid_ft_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    stk_mid_ft_mkdir     ( const char *pathname, int mode ) ;
      long    stk_mid_ft_rmdir     ( const char *pathname ) ;

      DIR   * stk_mid_ft_opendir   ( char *path ) ;
      int     stk_mid_ft_closedir  ( DIR *dirp ) ;
      struct dirent *stk_mid_ft_readdir ( DIR *dirp ) ;
      void    stk_mid_ft_rewinddir ( DIR *dirp ) ;


   /* ................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

