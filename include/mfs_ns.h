
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

#ifndef __MFS_NS_H__
#define __MFS_NS_H__

    // Includes
    #include "mfs_ns_common.h"
    #include "mfs_dbm_gdbm.h"
    #include "mfs_api.h"


    // API
    int  mfs_ns_init           ( void ) ;
    int  mfs_ns_finalize       ( void ) ;

    int  mfs_ns_insert         ( comm_t *wb,  int   ns_backend, char *srv_name, char *port_name ) ;
    int  mfs_ns_lookup         ( comm_t *wb,  int   ns_backend, char *srv_name, char *port_name, int *port_name_size ) ;
    int  mfs_ns_remove         ( comm_t *wb,  int   ns_backend, char *srv_name ) ;

    int  mfs_ns_get_portname   ( char *port_name,  int sd ) ;
    int  mfs_ns_split_portname ( char *port_name,  struct hostent **host, int *port ) ;

#endif

