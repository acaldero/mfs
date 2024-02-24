
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


#include "info_ns_mfs.h"


/*
 *  Name server API
 */

info_ns_mfs::info_ns_mfs ( void )
{
    this->ns_backend      = NS_USE_MFS ;
    this->ns_backend_name = "NS_MFS" ;
}

info_ns_mfs::~info_ns_mfs ( void )
{
}

int info_ns_mfs::insert ( char *srv_name, char *port_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name, "[COMM]: NULL srv_name :-(\n", -1) ;

    ret = -1 ;

	    /* TODO:
	     fd = mfs_api_dbmopen(wb, NS_FILE_NAME, GDBM_WRITER | GDBM_WRCREAT) ;
	     if (fd >= 0) {
	         ret = mfs_api_dbmstore(wb, fd, srv_name, strlen(srv_name)+1, port_name, strlen(port_name)+1) ;
	         mfs_api_dbmclose(wb, fd) ;
	     }
	     */

    // Return OK
    return ret ;
}

int info_ns_mfs::lookup ( char *srv_name, char *port_name, int *port_name_size )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(port_name, "[COMM]: NULL port_name :-(\n", -1) ;

    ret = -1 ;

	    /* TODO:
	     fd = mfs_api_dbmopen(wb, NS_FILE_NAME, GDBM_READER) ;
	     if (fd >= 0)
	     {
	         ret = mfs_api_dbmfetch(wb, fd, srv_name, strlen(srv_name)+1, &port_name, port_name_size) ;
	         mfs_api_dbmclose(wb, fd) ;
	     }
	     */

    // Return OK
    return ret ;
}

int info_ns_mfs::remove ( char *srv_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(\n", -1) ;

    ret = -1 ;

	    /* TODO:
	     fd = mfs_api_dbmopen(wb, NS_FILE_NAME, GDBM_WRITER) ;
	     if (fd >= 0)
	     {
	         ret = mfs_api_dbmdelete(wb, fd, srv_name, strlen(srv_name)+1) ;
	         mfs_api_dbmclose(wb, fd) ;
	     }
	     */

    // Return OK
    return ret ;
}


