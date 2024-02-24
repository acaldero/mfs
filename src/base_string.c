
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

#include "base_string.h"


//
// Strings
//

int base_strlen ( char *str1 )
{
    if (NULL == str1) {
	return 0 ;
    }

    return strlen(str1) ;
}

int base_str_equal ( char *str1, char *str2 )
{
    if (NULL == str1) {
        return (NULL == str2) ? 1 : -1 ;
    }
    if (NULL == str2) {
        return (NULL == str1) ? 1 : -1 ;
    }

    return (strcmp(str1, str2) == 0) ;
}

char * base_strdup ( char *str1 )
{
    if (NULL == str1) {
	return NULL ;
    }

    return strdup(str1) ;
}

int base_free_and_strdup ( char **ptr, char *str )
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

int base_str_prepare_pathname ( char **buff_data_sys, char *base_dirname, int local_rank, int pathname_length )
{
    int ret, buff_data_len ;

    // Check arguments...
    if (NULL == buff_data_sys) {
        mfs_print(DBG_WARNING, "[SERV_STUB] NULL buff_data_sys  :-/") ;
        return -1 ;
    }

    // Malloc for pathname...
    buff_data_len = strlen(base_dirname) + 20 + pathname_length ;  // base_dirname_value + '.' + local_rank + '/' + pathname

    (*buff_data_sys) = NULL ;
    ret = mfs_malloc(buff_data_sys, buff_data_len) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[BASE_LIB]: malloc(%d) fails :-(\n", buff_data_len) ;
	return -1 ;
    }

    // Print pathname...
    sprintf((*buff_data_sys), "%s/%d/", base_dirname, local_rank) ;

    // Return OK
    return 1 ;
}

