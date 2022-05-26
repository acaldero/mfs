
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

#ifndef _STK_FD_H_
#define _STK_FD_H_

    // Includes
    #include "base_lib.h"


    // Datatypes
    typedef struct {
         pthread_mutex_t   mutex ;
         int               hash_neltos ;
         void             *hash_eltos ;
         int              *been_used ;
         int              *file_fd ;
    } stk_fda_t ;


    // API
    int   stk_fda_init      ( stk_fda_t *des, int neltos, int elto_size ) ;
    int   stk_fda_finalize  ( stk_fda_t *des ) ;

    int   stk_fda_find_free ( stk_fda_t *des, int elto_size ) ;
    void  stk_fda_set_free  ( stk_fda_t *des, int fd ) ;
    int   stk_fda_is_free   ( stk_fda_t *des, int fd ) ;

    void *stk_fda_fd2fh     ( stk_fda_t *des, int fd, int elto_size ) ;

#endif

