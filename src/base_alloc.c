
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

#include "base_alloc.h"


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


