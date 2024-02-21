
#ifndef _STK_FD_H
#define _STK_FD_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "include/stk_fmeta.h"


   /* ... Types / Tipos ................................................. */

      typedef 
      struct struct_stk_fd_t stk_fd_t ;

      struct struct_stk_fd_t
      {
	 // internal data
         char     *xpn_fname ;
         int       xpn_fd[2] ;
         fmeta_t   xpn_fmeta ;
         int       xpni_fsd ;
         int       xpn_flag ;
         mode_t    xpn_mode ;

         stk_fs_t *xpn_fs ;
      } ;


   /* ... Functions / Funciones ......................................... */

      stk_fd_t  *stk_fd_new     ( void ) ;
      int        stk_fd_destroy ( stk_fd_t **fdi ) ;


   /* ................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

