
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


#include "info_ns_dbm.h"


/*
 *  Name server API
 */

info_ns_dbm::info_ns_dbm ( void )
{
    this->ns_backend      = NS_USE_DBM ;
    this->ns_backend_name = "NS_DBM" ;
}

info_ns_dbm::~info_ns_dbm ( void )
{
}

int info_ns_dbm::insert ( char *srv_name, char *port_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name, "[COMM]: NULL srv_name :-(\n", -1) ;

    ret = -1 ;
#ifdef HAVE_GDBM_H
	     mfs_dbm_gdbm fd1 ;
	     int flags ;

	     flags = GDBM_WRITER ;
	     if (access(NS_FILE_NAME, F_OK) != 0) {
	         flags = flags | GDBM_WRCREAT ;
             }

	     mfs_print(DBG_INFO, "[INFO_NS]: open '%s' + store k:%s,v:%s + close\n", NS_FILE_NAME, srv_name, port_name) ;
	     ret = fd1.open(NS_FILE_NAME, flags) ;
	     if (ret >= 0) {
	         ret = fd1.store(srv_name, strlen(srv_name) + 1, port_name, strlen(port_name) + 1) ;
	         fd1.close() ;
	     }

	     if (ret != 0) {
	         fprintf(stderr, "gdbm_store: %s\n", gdbm_strerror(gdbm_errno)) ;
	     }
#endif

    // Return OK
    return ret ;
}

int info_ns_dbm::lookup ( char *srv_name, char *port_name, int *port_name_size )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(port_name, "[COMM]: NULL port_name :-(\n", -1) ;

    ret = -1 ;
#ifdef HAVE_GDBM_H
	     mfs_dbm_gdbm fd1 ;

	     mfs_print(DBG_INFO, "[INFO_NS]: open '%s' + fetch k:%s + close\n", NS_FILE_NAME, srv_name) ;
	     ret = fd1.open(NS_FILE_NAME, GDBM_READER) ;
	     if (ret >= 0) {
	         ret = fd1.fetch((char *)srv_name, strlen(srv_name) + 1, port_name, port_name_size) ;
	         fd1.close() ;
	     }
#endif

    // Return OK
    return ret ;
}

int info_ns_dbm::remove ( char *srv_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(\n", -1) ;

    ret = -1 ;
#ifdef HAVE_GDBM_H
	     mfs_dbm_gdbm fd1 ;

	     mfs_print(DBG_INFO, "[INFO_NS]: open '%s' + remove k:%s + close\n", NS_FILE_NAME, srv_name) ;
	     ret = fd1.open(NS_FILE_NAME, GDBM_WRITER) ;
	     if (ret >= 0) {
	         ret = fd1.del((char *)srv_name, strlen(srv_name) + 1) ;
	         fd1.close() ;
	     }
#endif

    // Return OK
    return ret ;
}


