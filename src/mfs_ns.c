
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


#include "mfs_ns.h"


/*
 *  Name server API
 */

int  mfs_ns_init ( void )
{
    int  ret ;

    //
    // TODO
    //
    ret = -1 ;

    // Return OK/KO
    return ret ;
}

int  mfs_ns_finalize ( void )
{
    int  ret ;

    //
    // TODO
    //
    ret = -1 ;

    // Return OK/KO
    return ret ;
}

int mfs_ns_insert ( int ns_backend, char *srv_name, char *port_name )
{
    int    ret ;

    // Check params...
    if (NULL == srv_name) {
	return -1 ;
    }

    switch (ns_backend)
    {
        case NS_USE_MFS:
	     // TODO:
             ret = -1 ;
	     // TODO:
             break ;

        case NS_USE_DBM:
#ifdef HAVE_GDBM_H
             GDBM_FILE fd ;
             datum key, value ;

	     ret = -1 ;
	     fd  = gdbm_open("ns.gdbm", 0, GDBM_WRITER | GDBM_WRCREAT, 0700, NULL) ;
	     if (NULL != fd)
	     {
	         // Build key+val
	         key.dptr    = (char *)srv_name ;
	         key.dsize   = strlen(srv_name) + 1;
	         value.dptr  = (char *)port_name ;
	         value.dsize = strlen(port_name) + 1;

	         // Store key+val
	         ret = gdbm_store(fd, key, value, GDBM_REPLACE) ;
		 gdbm_close(fd) ;
	     }
#endif
             break ;

        case NS_USE_FILE:
	     FILE *f;
	     char host[255];

	     f = fopen("ns.txt", "a+");
	     if (f != NULL) {
	         gethostname(host, 255);
	         // fprintf(f, "%s %s %d\r\n", name, host, port);
	         fclose(f);
	     }
	     ret = (f != NULL) ? 1 : -1 ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on ns_backend(%d).\n", ns_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_ns_lookup ( int ns_backend, char *srv_name, char *port_name )
{
    int    ret ;

    // Check params...
    if (NULL == srv_name) {
	return -1 ;
    }

    switch (ns_backend)
    {
        case NS_USE_MFS:
	     // TODO:
             ret = -1 ;
	     // TODO:
             break ;

        case NS_USE_DBM:
#ifdef HAVE_GDBM_H
             GDBM_FILE fd ;
             datum key, value ;

	     ret = -1 ;
	     fd  = gdbm_open("ns.gdbm", 0, GDBM_READER, 0700, NULL) ;
	     if (NULL != fd)
	     {
	         key.dptr  = (char *)srv_name ;
	         key.dsize = strlen(srv_name) + 1;

                 value = gdbm_fetch(fd, key) ;
	         strcpy(port_name, value.dptr) ;

		 gdbm_close(fd) ;
	     }
#endif
             break ;

        case NS_USE_FILE:
	     FILE *f;
	     char host[255];

	     f = fopen("ns.txt", "a+");
	     if (f != NULL) {
	         gethostname(host, 255);
	         /// TODO: search ...
		 //fscanf(f, "%s %s %d\r\n", name, host, port);
	         fclose(f);
	     }
	     ret = (f != NULL) ? 1 : -1 ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on ns_backend(%d).\n", ns_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_ns_remove ( int ns_backend, char *srv_name )
{
    int    ret ;

    // Check params...
    if (NULL == srv_name) {
	return -1 ;
    }

    switch (ns_backend)
    {
        case NS_USE_MFS:
	     // TODO:
             ret = -1 ;
	     // TODO:
             break ;

        case NS_USE_DBM:
#ifdef HAVE_GDBM_H
             GDBM_FILE fd ;
             datum key ;

	     ret = -1 ;
	     fd  = gdbm_open("ns.gdbm", 0, GDBM_WRITER, 0700, NULL) ;
	     if (NULL != fd)
	     {
	         key.dptr  = (char *)srv_name ;
	         key.dsize = strlen(srv_name) + 1;

                 ret = gdbm_delete(fd, key) ;
		 gdbm_close(fd) ;
	     }
#endif
             break ;

        case NS_USE_FILE:
	     FILE *f;
	     char host[255];

	     f = fopen("ns.txt", "a+");
	     if (f != NULL) {
	         gethostname(host, 255);
	         /// TODO: search ...
		 //fscanf(f, "%s %s %d\r\n", name, host, port);
	         fclose(f);
	     }
	     ret = (f != NULL) ? 1 : -1 ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on ns_backend(%d).\n", ns_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_ns_get_portname ( char *port_name, int sd )
{
	char               my_ip[16] ;
        unsigned int       my_port ;
        struct sockaddr_in my_addr ;
        int                sockfd ;
        socklen_t          len ;

	// initialize variables
        len = sizeof(my_addr) ;
        bzero(&my_addr, len) ;
	my_port = 0 ;

	// get addr+port
        getsockname(sd, (struct sockaddr *)&(my_addr), &(len)) ;
        inet_ntop(AF_INET, &(my_addr.sin_addr), my_ip, sizeof(my_ip)) ;
        my_port = ntohs(my_addr.sin_port) ;

	// set portname
	sprintf(port_name, "%16s:%d", my_ip, my_port) ;

        // Return OK
        return 1 ;
}

int mfs_ns_split_portname ( char *port_name, struct hostent **host, int *port )
{
	char *pch ;
	char  srv_host[MAXPATHLEN] ;
	char  srv_port[MAXPATHLEN] ;

	// copy default values...
	strcpy(srv_host, port_name) ;
	strcpy(srv_port, "12345") ;

	// if "host:port" -> host\0port
	pch = strchr(srv_host, ':') ;
	if (NULL != pch) {
	    *pch = '\0' ;
	    pch++ ;
	    strcpy(srv_port, pch) ;
	}

	// translate
	*port = atoi(srv_port) ;
	*host = gethostbyname(srv_host) ;
	if (*host == NULL) {
	    mfs_print(DBG_ERROR, "[COMM]: gethostbyname(%s) fails :-(", srv_host) ;
	    return -1 ;
	}

        // Return OK
        return 1 ;
}

