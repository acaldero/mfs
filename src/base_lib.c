
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#include "base_lib.h"


//
// Print for debug
//

int mfs_print ( int src_type, char *src_fname, long src_line, FILE *fd, char *msg_fmt, ... )
{
   va_list valist ;
   int ret ;

   va_start(valist, msg_fmt) ;
   switch (src_type)
   {
        case  3:
#ifdef DEBUG
              fprintf(fd, "[%s:%4ld] [INFO] ", src_fname, src_line) ;
              ret = vfprintf(fd, msg_fmt, valist) ;
#endif
     	      break;

        case  2:
#ifdef DEBUG
              fprintf(fd, "[%s:%4ld] [WARN] ", src_fname, src_line) ;
              ret = vfprintf(fd, msg_fmt, valist) ;
#endif
     	      break;

        case  1:
#ifdef DEBUG
              fprintf(fd, "[%s:%4ld] [ERROR] ", src_fname, src_line) ;
              ret = vfprintf(fd, msg_fmt, valist) ;
#endif
     	      break;

        default:
              ret = vfprintf(fd, msg_fmt, valist) ;
	      break;
   }
   va_end(valist);

   return ret ;
}


//
// Thread
//

int mfs_get_thread_id ( long *th_id )
{
    // check arguments
    if (NULL == th_id) {
	return -1 ;
    }

    // get thread id
    *th_id = (long)pthread_self() ;

    // Return OK
    return 1 ;
}


