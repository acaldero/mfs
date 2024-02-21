
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

#ifndef __MFS_NS_H__
#define __MFS_NS_H__

    // Includes
    #include "info_ns_common.h"
    #include "mfs_dbm_gdbm.h"
    #include "mfs_dbm_tdb.h"
    #include "mfs_api.h"


    // API
    int  info_ns_init           ( void ) ;
    int  info_ns_finalize       ( void ) ;

    int  info_ns_insert         ( comm_t *wb,  int   ns_backend, char *srv_name, char *port_name ) ;
    int  info_ns_lookup         ( comm_t *wb,  int   ns_backend, char *srv_name, char *port_name, int *port_name_size ) ;
    int  info_ns_remove         ( comm_t *wb,  int   ns_backend, char *srv_name ) ;

    int  info_ns_get_portname   ( char *port_name,  int sd ) ;
    int  info_ns_split_portname ( char *port_name,  struct hostent **host, int *port ) ;

#endif

