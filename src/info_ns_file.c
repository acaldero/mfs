
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


#include "info_ns_file.h"


/*
 *  Name server API
 */

info_ns_file::info_ns_file ( void )
{
    this->ns_backend      = NS_USE_FILE ;
    this->ns_backend_name = "NS_FILE" ;
}

info_ns_file::~info_ns_file ( void )
{
}

int info_ns_file::insert ( char *srv_name, char *port_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name, "[COMM]: NULL srv_name :-(\n", -1) ;

	     FILE *f ;
	     char host[255] ;
	     int  port ;

	     port = 0 ;
	     f = fopen(NS_FILE_NAME, "a+") ;
	     if (f != NULL) {
	         gethostname(host, 255) ;
	         fprintf(f, "%s;%s;%s:%d\n", srv_name, port_name, host, port) ; // port <- TODO
	         fclose(f) ;
	     }
	     ret = (f != NULL) ? 1 : -1 ;

    // Return OK
    return ret ;
}

int info_ns_file::lookup ( char *srv_name, char *port_name, int *port_name_size )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(port_name, "[COMM]: NULL port_name :-(\n", -1) ;

	     FILE *f ;
	     char srv[255] ;
	     char host[255] ;
	     int  port, is_found ;

	     is_found = 0 ;
	     f = fopen(NS_FILE_NAME, "r") ;
	     if (f != NULL)
	     {
		 while (!feof(f))
		 {
	            fscanf(f, "%s;%s;%s:%d\n", srv, port_name, host, &port) ;
		    if (!strcmp(srv_name, srv)) {
		        is_found = 1 ;
			break ;
		    }
                 }
	         fclose(f) ;
	     }
	     ret = (is_found != 0) ? 1 : -1 ;

    // Return OK
    return ret ;
}

int info_ns_file::remove ( char *srv_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(\n", -1) ;

	     FILE *f;
	     char host[255];

	     f = fopen(NS_FILE_NAME, "a+");
	     if (f != NULL) {
	         gethostname(host, 255);
	         /// TODO: search ...
		 //fscanf(f, "%s %s %d\r\n", name, host, port);
	         fclose(f);
	     }
	     ret = (f != NULL) ? 1 : -1 ;

    // Return OK
    return ret ;
}


