
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MFS.
 *
 *  MFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mfs_lib.h"


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
// Time
//

long mfs_get_time ( void )
{
    struct timeval timenow ;

    // get timestamp
    gettimeofday(&timenow, NULL) ;

    // return timestamp
    return (long)timenow.tv_sec * 1000 + (long)timenow.tv_usec / 1000 ;
}


//
// Memory
//

int mfs_malloc ( char **ptr, long size )
{
    // check arguments
    if (NULL == ptr) {
	return -1 ;
    }

    // alloc buffer
    (*ptr) = (char *)malloc(size) ;
    if (NULL == (*ptr)) {
	perror("malloc: ") ;
	return -1 ;
    }

    memset((*ptr), 0, size) ;

    // Return OK
    return 1 ;
}

int mfs_free ( char **ptr )
{
    // check arguments
    if (NULL == ptr) {
	return -1 ;
    }

    // free buffer
    if (NULL != *ptr)
    {
        free(*ptr) ;
        *ptr = NULL ;
    }

    // Return OK
    return 1 ;
}

int mfs_realloc ( char **ptr, long new_size )
{
    char *new_ptr ;

    // check arguments
    if (NULL == ptr) {
	return -1 ;
    }

    // alloc buffer
    new_ptr = (char *)realloc((*ptr), new_size) ;
    if (NULL == new_ptr) {
	return -1 ;
    }

    (*ptr) = new_ptr ;

    // Return OK
    return 1 ;
}

int mfs_free_and_strdup ( char **ptr, char *str )
{
    // check arguments
    if (NULL == ptr) {
	return -1 ;
    }

    free(*ptr) ;
    *ptr = strdup(str) ;

    // Return OK
    return 1 ;
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

