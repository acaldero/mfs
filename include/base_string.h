
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

#ifndef __BASE_STRING_H__
#define __BASE_STRING_H__

    // Includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <strings.h>
    #include <unistd.h>

    #include "base_lib.h"
    #include "base_alloc.h"


    // Strings
    int    base_strlen         ( char *str1 ) ;
    int    base_str_equal      ( char *str1, char *str2 ) ;
    char * base_strdup         ( char *str1 ) ;
    int    base_free_and_strdup ( char **ptr, char *str ) ;
    int    base_str_prepare_pathname ( char **buff_data_sys, char *base_dirname, int local_rank, int pathname_length ) ;

#endif

