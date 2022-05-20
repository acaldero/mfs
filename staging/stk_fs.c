
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of XPNlite.
 *
 *  XPNlite is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPNlite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with XPNlite.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


   /* ... Include / Inclusion ........................................... */

      #include "include/stk_fs.h"


   /* ... Functions / Funciones ......................................... */

      int stk_fs_init ( stk_fs_t *fsi, stk_fs_t *low_fsi )
      {
	  int ret ;

	  /* check params */
	  if (NULL == fsi) {
	      return -1 ;
	  }

	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p, low_fsi:%p", fsi, low_fsi) ;

	  /* initialize interface */
	  fsi->fsi_name       = NULL ;
	  fsi->low_fsi        = low_fsi ;

	  // init/finalize
	  fsi->fsi_init       = NULL ;
          fsi->fsi_finalize   = NULL ;
	  // file API
          fsi->fsi_open       = NULL ;
          fsi->fsi_creat      = NULL ;
          fsi->fsi_close      = NULL ;
          fsi->fsi_read       = NULL ;
          fsi->fsi_write      = NULL ;
          fsi->fsi_lseek      = NULL ;
	  // directory API
          fsi->fsi_opendir    = NULL ;
          fsi->fsi_closedir   = NULL ;
          fsi->fsi_readdir    = NULL ;
          fsi->fsi_mkdir      = NULL ;
          fsi->fsi_rmdir      = NULL ;
          fsi->fsi_rewinddir  = NULL ;

	  XPN_DEBUG_END_CUSTOM("fsi:%p, low_fsi:%p -> %d", fsi, low_fsi, ret) ;

	  /* return ok|ko */
	  return ret ;
      }

      int stk_fs_finalize ( stk_fs_t *fsi )
      {
	  int ret ;

	  /* check params */
	  if (NULL == fsi) {
	      return -1 ;
	  }

	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p", fsi) ;

	  /* unregister xpnsi_null interface */
	  if (NULL != fsi->fsi_name) {
	      free(fsi->fsi_name) ;
	      fsi->fsi_name   = NULL ;
	  }

	  // init/destroy
	  fsi->fsi_init       = NULL ;
          fsi->fsi_finalize   = NULL ;
	  // file API
          fsi->fsi_open       = NULL ;
          fsi->fsi_creat      = NULL ;
          fsi->fsi_close      = NULL ;
          fsi->fsi_read       = NULL ;
          fsi->fsi_write      = NULL ;
          fsi->fsi_lseek      = NULL ;
	  // directory API
          fsi->fsi_opendir    = NULL ;
          fsi->fsi_closedir   = NULL ;
          fsi->fsi_readdir    = NULL ;
          fsi->fsi_mkdir      = NULL ;
          fsi->fsi_rmdir      = NULL ;
          fsi->fsi_rewinddir  = NULL ;

	  XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	  /* return ok|ko */
	  return ret ;
      }

      stk_fs_t  *stk_fs_new ( void )
      {
	  stk_fs_t *stk_fs_aux ;

	  XPN_DEBUG_BEGIN();

	  /* alloc a new xpni_fsi */
	  stk_fs_aux = (stk_fs_t *)malloc(sizeof(stk_fs_t));

	  /* set to default values */
	  if (NULL != stk_fs_aux) {
              stk_fs_init(stk_fs_aux, NULL) ;
	  }

	  XPN_DEBUG_END();

	  /* return this new element */
	  return stk_fs_aux ;
      }

      int stk_fs_destroy ( stk_fs_t **fsi )
      {
	  XPN_DEBUG_BEGIN();

	  if (*fsi != NULL)
	  {
	      // free fsi fields...
              stk_fs_finalize(*fsi) ;

	      // free fsi itself...
	      free(*fsi) ;
	      *fsi = NULL ;
	  }

	  XPN_DEBUG_END();

	  /* return 'ok' */
	  return 1 ;
      }


   /* ................................................................... */

