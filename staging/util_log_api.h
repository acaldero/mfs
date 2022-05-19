
#ifndef _UTIL_LOG_API_H
#define _UTIL_LOG_API_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"


   /* ... Functions / Funciones ......................................... */

      int util_log_elog
	  (
             /*IN*/  struct timeval *t1,
             /*IN*/  struct timeval *t2,
             /*IN*/  char           *action,
             /*IN*/  int             object,
             /*IN*/  int             offset,
             /*IN*/  int             size
	  ) ;

      int util_log_vprintf
	  (
	     /*IN*/  char           *ename,
             ...
	  ) ;


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

