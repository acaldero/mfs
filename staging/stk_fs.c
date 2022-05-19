
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

      stk_fs_t  *stk_fs_new ( void )
      {
	  stk_fs_t *stk_fs_aux ;

	  XPN_DEBUG_BEGIN();

	  /* alloc a new xpni_fsi */
	  stk_fs_aux = (stk_fs_t *)malloc(sizeof(stk_fs_t));

	  /* set to default values */
	  if (NULL != stk_fs_aux) {
	      memset(stk_fs_aux,0,sizeof(stk_fs_t));
	  }

	  XPN_DEBUG_END();

	  /* return this new element */
	  return stk_fs_aux ;
      }

      int stk_fs_destroy ( stk_fs_t **fsi )
      {
	  XPN_DEBUG_BEGIN();

	  if (*fsi != NULL) {
	      free(*fsi) ;
	      *fsi = NULL ;
	  }

	  XPN_DEBUG_END();

	  /* return 'ok' */
	  return 1 ;
      }


   /* ................................................................... */

