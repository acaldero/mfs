
#ifndef _XPN_SB_XPN_H
#define _XPN_SB_XPN_H

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
      int xpnsb_xpn_register   ( xpni_fsi_t *fsi ) ;
      int xpnsb_xpn_unregister ( xpni_fsi_t *fsi ) ;

      // init + destroy
      int xpnsb_xpn_init    () ;
      int xpnsb_xpn_destroy () ;

      // open + close + sread + swrite
      int     xpnsb_xpn_creat  ( char *path, mode_t mode ) ;
      int     xpnsb_xpn_open   ( char *path, int flags, mode_t mode ) ;
      int     xpnsb_xpn_close  ( int fd ) ;
      off_t   xpnsb_xpn_lseek  ( int fd, off_t offset, int flag ) ;
      ssize_t xpnsb_xpn_swrite ( int fd, void *buffer, size_t size, off_t offset ) ;
      ssize_t xpnsb_xpn_sread  ( int fd, void *buffer, size_t size, off_t offset ) ;

      // import + export
      int xpnsb_xpn_exportFile ( __attribute__((__unused__)) int fd ) ;
      int xpnsb_xpn_importFile ( __attribute__((__unused__)) int fd ) ;

      // opendir + closedir + readdir + rewind
      DIR * xpnsb_xpn_opendir ( char *path ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

