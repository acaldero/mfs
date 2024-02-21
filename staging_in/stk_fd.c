
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

      #include "include/stk_fd.h"


   /* ... Functions / Funciones ......................................... */

      stk_fd_t  *stk_fd_new ( void )
      {
	  stk_fd_t *stk_fd_aux ;

	  XPN_DEBUG_BEGIN();

	  /* alloc a new xpni_fdi */
	  stk_fd_aux = (stk_fd_t *)malloc(sizeof(stk_fd_t));

	  /* set to default values */
	  if (NULL != stk_fd_aux) {
	      memset(stk_fd_aux,0,sizeof(stk_fd_t));
	  }

	  XPN_DEBUG_END();

	  /* return this new element */
	  return stk_fd_aux ;
      }

      int stk_fd_destroy ( stk_fd_t **fdi )
      {
	  XPN_DEBUG_BEGIN();

	  if (*fdi != NULL) {
	      free(*fdi) ;
	      *fdi = NULL ;
	  }

	  XPN_DEBUG_END();

	  /* return 'ok' */
	  return 1 ;
      }


   /* ................................................................... */

