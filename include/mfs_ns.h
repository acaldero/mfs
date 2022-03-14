
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MPI_PFS.
 *
 *  MPI_PFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MPI_PFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MPI_PFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MFS_NS_H__
#define __MFS_NS_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_dbm_gdbm.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>


    // File protocol
    #define NS_USE_MFS   1
    #define NS_USE_DBM   2
    #define NS_USE_FILE  3

    // Consts
    #define MAXPATHLEN (1024)


    // API
    int  mfs_ns_init           ( void ) ;
    int  mfs_ns_finalize       ( void ) ;
    int  mfs_ns_insert         ( int   ns_backend, char *srv_name, char *port_name ) ;
    int  mfs_ns_lookup         ( int   ns_backend, char *srv_name, char *port_name ) ;
    int  mfs_ns_remove         ( int   ns_backend, char *srv_name ) ;
    int  mfs_ns_get_portname   ( char *port_name,  int sd ) ;
    int  mfs_ns_split_portname ( char *port_name,  struct hostent **host, int *port ) ;

#endif

