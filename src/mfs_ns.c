
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


#include "mfs_ns.h"


/*
 *  Name server API
 */

int  mfs_ns_init ( void )
{
    // Return OK/KO
    return 1 ;
}

int  mfs_ns_finalize ( void )
{
    // Return OK/KO
    return 1 ;
}

int mfs_ns_insert ( comm_t *wb, int ns_backend, char *srv_name, char *port_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name, "[COMM]: NULL srv_name :-(", -1) ;

    ret = -1 ;
    switch (ns_backend)
    {
        case NS_USE_MFS:
	     fd = mfs_api_dbmopen(wb, NS_FILE_NAME, GDBM_WRITER | GDBM_WRCREAT) ;
	     if (fd >= 0) {
	         ret = mfs_api_dbmstore(wb, fd, srv_name, strlen(srv_name)+1, port_name, strlen(port_name)+1) ;
	         mfs_api_dbmclose(wb, fd) ;
	     }
             break ;

        case NS_USE_DBM:
#ifdef HAVE_GDBM_H
             GDBM_FILE fd ;

	     ret = mfs_dbm_gdbm_open(&fd, NS_FILE_NAME, GDBM_WRITER | GDBM_WRCREAT) ;
	     if (ret >= 0) {
	         ret = mfs_dbm_gdbm_store(fd, srv_name, strlen(srv_name) + 1, port_name, strlen(port_name) + 1) ; 
	         mfs_dbm_gdbm_close(fd) ;
	     }
#endif
             break ;

        case NS_USE_FILE:
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
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on ns_backend(%d).\n", ns_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_ns_lookup ( comm_t *wb, int ns_backend, char *srv_name, char *port_name, int *port_name_size )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,        "[COMM]: NULL wb        :-(", -1) ;
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(", -1) ;
    NULL_PRT_MSG_RET_VAL(port_name, "[COMM]: NULL port_name :-(", -1) ;

    ret = -1 ;
    switch (ns_backend)
    {
        case NS_USE_MFS:
	     fd = mfs_api_dbmopen(wb, NS_FILE_NAME, GDBM_READER) ;
	     if (fd >= 0)
	     {
	         ret = mfs_api_dbmfetch(wb, fd, srv_name, strlen(srv_name)+1, &port_name, port_name_size) ;
	         mfs_api_dbmclose(wb, fd) ;
	     }
             break ;

        case NS_USE_DBM:
#ifdef HAVE_GDBM_H
             GDBM_FILE fd ;

	     ret = mfs_dbm_gdbm_open(&fd, NS_FILE_NAME, GDBM_READER) ;
	     if (ret >= 0) {
	         ret = mfs_dbm_gdbm_fetch(fd, (char *)srv_name, strlen(srv_name) + 1, port_name, port_name_size) ;
	         mfs_dbm_gdbm_close(fd) ;
	     }
#endif
             break ;

        case NS_USE_FILE:
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
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on ns_backend(%d).\n", ns_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_ns_remove ( comm_t *wb, int ns_backend, char *srv_name )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(srv_name,  "[COMM]: NULL srv_name  :-(", -1) ;

    ret = -1 ;
    switch (ns_backend)
    {
        case NS_USE_MFS:
	     fd = mfs_api_dbmopen(wb, NS_FILE_NAME, GDBM_WRITER) ;
	     if (fd >= 0)
	     {
	         ret = mfs_api_dbmdelete(wb, fd, srv_name, strlen(srv_name)+1) ;
	         mfs_api_dbmclose(wb, fd) ;
	     }
             break ;

        case NS_USE_DBM:
#ifdef HAVE_GDBM_H
             GDBM_FILE fd ;

	     ret = mfs_dbm_gdbm_open(&fd, NS_FILE_NAME, GDBM_WRITER) ;
	     if (ret >= 0) {
	         ret = mfs_dbm_gdbm_delete(fd, (char *)srv_name, strlen(srv_name) + 1) ;
	         mfs_dbm_gdbm_close(fd) ;
	     }
#endif
             break ;

        case NS_USE_FILE:
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
	sprintf(srv_port, "%s", DEFAULT_PORT) ;

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

