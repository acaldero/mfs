
#ifndef _XPN_SI_LOG_H
#define _XPN_SI_LOG_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "all_system.h"
      #include "base/time_misc.h"
      #include "include/xpni_log_elog.h"
      #include "include/xpni_fit.h"
      #include "include/xpni_fsit.h"
      #include "include/xpni_file.h"
      #include "include/xpni_lowfsi.h"


   /* ... Functions / Funciones ......................................... */

      int     xpni_log_init    ( ) ;
      int     xpni_log_destroy ( ) ;

      int     xpni_log_register   ( xpni_fsi_t *fsi ) ;
      int     xpni_log_unregister ( xpni_fsi_t *fsi ) ;

      int     xpni_log_importFile ( int fd ) ;
      int     xpni_log_exportFile ( int fd ) ;

      int     xpni_log_open    ( int fd ) ;
      int     xpni_log_creat   ( int fd ) ;
      int     xpni_log_close   ( int fd ) ;

      ssize_t xpni_log_sread   ( int fd, void *buffer, off_t offset, size_t size ) ;
      ssize_t xpni_log_swrite  ( int fd, void *buffer, off_t offset, size_t size ) ;

      off_t   xpni_log_lseek   ( int fd, off_t offset, int flag ) ;

      DIR    *xpni_log_opendir	( int dd ) ;

      /*#NewInterfaceFunction.AsFunction3#*/


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

