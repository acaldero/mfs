
#ifndef _XPNI_FT_H
#define _XPNI_FT_H

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
      int  xpnsi_ft_register   ( xpni_fsi_t *fsi ) ;
      int  xpnsi_ft_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int     xpnsi_ft_init    () ;
      int     xpnsi_ft_destroy () ;

      // File API
      int     xpnsi_ft_creat  ( char *path, mode_t mode ) ;
      int     xpnsi_ft_open   ( char *path, int flags, mode_t mode ) ;
      int     xpnsi_ft_close  ( int fd ) ;
      off_t   xpnsi_ft_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t xpnsi_ft_write  ( int fd, void *buffer, size_t size ) ;
      ssize_t xpnsi_ft_read   ( int fd, void *buffer, size_t size ) ;

      // Directory API
      long    xpnsi_ft_mkdir     ( const char *pathname, int mode ) ;
      long    xpnsi_ft_rmdir     ( const char *pathname ) ;

      DIR   * xpnsi_ft_opendir   ( char *path ) ;
      int     xpnsi_ft_closedir  ( DIR *dirp ) ;
      struct dirent *xpnsi_ft_readdir ( DIR *dirp ) ;
      void    xpnsi_ft_rewinddir ( DIR *dirp ) ;


   /* ................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

