
#ifndef _XPN_SB_POSIX_H
#define _XPN_SB_POSIX_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "all_system.h"
      #include "xpni/common/xpni_fit.h"
      #include "xpni/common/xpni_fsit.h"
      #include "xpni/common/xpni_file.h"
      #include "xpni/common/xpni_lowfsi.h"


   /* ... Functions / Funciones ......................................... */

      // register + unregister
      int xpnsb_posix_register   ( xpni_fsi_t *fsi ) ;
      int xpnsb_posix_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int xpnsb_posix_init    () ;
      int xpnsb_posix_destroy () ;

      // open + close + sread + swrite
      int     xpnsb_posix_creat  ( char *path, mode_t mode ) ;
      int     xpnsb_posix_open   ( char *path, int flags, mode_t mode ) ;
      int     xpnsb_posix_close  ( int fd ) ;
      off_t   xpnsb_posix_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t xpnsb_posix_swrite ( int fd, void *buffer, size_t size, off_t offset ) ;
      ssize_t xpnsb_posix_sread  ( int fd, void *buffer, size_t size, off_t offset ) ;

      // import + export
      int xpnsb_posix_exportFile ( __attribute__((__unused__)) int fd ) ;
      int xpnsb_posix_importFile ( __attribute__((__unused__)) int fd ) ;

      // opendir + closedir + readdir + rewind
      DIR * xpnsb_posix_opendir ( char *path ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

