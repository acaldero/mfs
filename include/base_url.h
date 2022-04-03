
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

#ifndef __BASE_URL_H__
#define __BASE_URL_H__

    // Includes
    #include "base_lib.h"
    #include "base_socket.h"


    // Datatypes
    typedef struct {
	char *name ;
	int   port ;
    } base_service_t ;

    typedef struct {
        char  *protocol ;
        char  *user ;
        char  *machine ;
        int    port ;
        char  *file ;
        char  *relative ;
        char  *params ;
    } base_url_t ;


    // API
    int base_url_parseURL ( base_url_t *urlbase, char *urlstr ) ;
    int base_url_freeURL  ( base_url_t *urlbase ) ;

    base_url_t *base_url_dup  ( base_url_t  *base_url ) ;
    int         base_url_free ( base_url_t **base_url ) ;

#endif

