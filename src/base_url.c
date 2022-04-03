
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


#include "base_url.h"

/*
 *  Auxiliar functions
 */

base_service_t  base_url_protocols[] = 
                {
		    { "http",          80 },
		    { "ftp",           21 },
		    { "file",           0 },
		    { "tcpServer",    110 },
		    { "mpiServer",  12345 },
		    { NULL, 0 }
                } ;

int    base_url_Match_protocol ( char **protocol, char **str )
{
        int i, ret ;

        for (i=0; base_url_protocols[i].name != NULL; i++)
            {
              ret = strncmp( (*str),
                             base_url_protocols[i].name,
                             base_strlen(base_url_protocols[i].name) ) ;
              if (ret == 0)
                 {
                   (*protocol) = base_strdup(base_url_protocols[i].name) ;
                   (*str)      = (*str) + base_strlen(base_url_protocols[i].name) ;
                   if ( (**str) == ':' )
                        (*str) ++ ;
                   if ( (**str) == '/' )
                        (*str) ++ ;
                   if ( (**str) == '/' )
                        (*str) ++ ;
                   return 1 ;
                 }
            }

        (*protocol) = base_strdup("http") ;
        return 1 ;
}

int    base_url_Match_user ( char **user, char **str )
{
        char *pch ;

	// Try to match localhost@user:pass...
        pch = strchr((*str), '@') ;
        if (pch == NULL) {
            (*user) = NULL ;
            return 1 ;
        }

        pch[0] = '\0' ;
        (*user) = base_strdup((*str)) ;
        (*str) = pch + 1 ;
        return 1 ;
}

int    base_url_Match_machine ( char **machine, char *protocol, char **str )
{
        char *pch1, *pch2 ;

        /* 
         *  file
         */
        if ( base_str_equal(protocol, "file") )
        {
             int ret ;

             (*machine) = base_strdup("localhost") ;
             ret = strncmp((*str), "localhost", base_strlen("localhost")) ;
             if (ret == 0) {
                 (*str) = (*str) + base_strlen("localhost") ;
	     }
             return 1 ;
        }

        /* 
         *  http || ftp 
         */
        pch1 = strchr((*str),'/') ;
        pch2 = strchr((*str),':') ;
        if (pch2 == NULL)
        {
             if (pch1 == NULL)
             {
                 (*machine) = base_strdup((*str)) ;
                 (*str) = (*str) + base_strlen((*str)) ;
                 return 1 ;
             }
             if (pch1 != NULL)
             {
                 pch1[0] = '\0' ;
                 (*machine) = base_strdup((*str)) ;
                 (*str) = pch1 ;
                 pch1[0] = '/' ;
                 return 1 ;
             }
        }
        if (pch2 != NULL)
        {
            pch2[0] = '\0' ;
            (*machine) = base_strdup((*str)) ;
            pch2[0] = ':' ;
            (*str) = pch2 ;
            return 1 ;
        }

	// Default value
        (*machine) = NULL ;
        return 1 ;
}

int    base_url_Match_port ( int *port, char *protocol, char **str )
{
        char *pch1, *pch2 ;

        /* default port... */
        (*port) = htons(80) ;
        for (int i=0; base_url_protocols[i].name != NULL; i++)
        {
             if (base_str_equal(protocol, base_url_protocols[i].name)) {
                 (*port) = htons(base_url_protocols[i].port) ;
	     }
        }

        /* scanning port... */
        pch1 = strchr((*str),':') ;
        if (pch1 == NULL) {
             return 1 ;
        }

        pch1 ++ ; /* skip ':' */
        pch2 = strchr(pch1,'/') ;
        if (pch2 == NULL)
        {
             (*port) = htons(atoi(pch1)) ;
             (*str)  = (*str) + base_strlen((*str)) ;
             return 1 ;
        }
        if (pch2 != NULL)
        {
             pch2[0] = '\0' ;
             (*port) = htons(atoi(pch1)) ;
             pch2[0] = '/' ;
             (*str)  = pch2 ;
             return 1 ;
        }

        return 1 ;
}

int    base_url_Match_file (  char **file, char **str )
{
        char *pch1 ;

	// if *str begins with "/." then skip leading /
	if (!strncmp(*str, "/.", 2)) {
	     (*str) ++ ;
	}

        pch1 = strchr((*str),'#') ;
        if (pch1 != NULL)
        {
             pch1[0] = '\0' ;
             (*file) = base_strdup((*str)) ;
             (*str)  = (*str) + base_strlen(pch1) ;
             pch1[0] = '#' ;
             return 1 ;
        }

        pch1 = strchr((*str),'?') ;
        if (pch1 != NULL)
        {
             pch1[0] = '\0' ;
             (*file) = base_strdup((*str)) ;
             pch1[0] = '?' ;
             (*str)  = pch1 ;
             return 1 ;
        }

        /* ... all is file ... */
        if ((*str)[0] != '\0')
        {
             (*file) = base_strdup((*str)) ;
             (*str)  = (*str) + base_strlen((*str)) ;
             return 1 ;
        }

        (*file) = base_strdup("/") ;
        return 1 ;
}

int base_url_Match_relative ( char **relative, char **str )
{
        char *pch1, *pch2 ;

        (*relative) = NULL ;
        pch1 = strchr((*str),'#') ;
        if (pch1 == NULL)
             return 1 ;

        pch1 ++ ; /* skip '#' */
        pch2 = strchr(pch1,'?') ;
        if (pch2 == NULL)
           {
             (*relative) = base_strdup((*str)) ;
             (*str)      = (*str) + base_strlen((*str)) ;
             return 1 ;
           }

        if (pch2 != NULL)
           {
             pch2[0]     = '\0' ;
             (*relative) = base_strdup(pch1) ;
             (*str)      = pch2 ;
             pch2[0]     = '?' ;
             return 1 ;
           }

        return 1 ;
}

int base_url_Match_params ( char **params, char **str )
{
        char *pch1 ;

        (*params) = NULL ;
        pch1 = strchr((*str),'?') ;
        if (pch1 == NULL) {
            return 1 ;
	}

        pch1 ++ ; /* skip '?' */
        (*params) = base_strdup(pch1) ;
        (*str)    = (*str) + base_strlen((*str)) ;
        return 1 ;
}

int    base_url_Match_url ( char    **protocol,
                            char    **user,
                            char    **machine,
                            int      *port,
                            char    **file,
                            char    **relative,
                            char    **params,
                            char    **str )
{
        int ok ;

        ok = base_url_Match_protocol(protocol, str) ;
        if (0 == ok) return (0) ;

        ok = base_url_Match_user(user, str) ;
        if (0 == ok) return (0) ;

        ok = base_url_Match_machine(machine, *protocol, str) ;
        if (0 == ok) return (0) ;

        ok = base_url_Match_port(port, *protocol, str) ;
        if (0 == ok) return (0) ;

        ok = base_url_Match_file(file, str) ;
        if (0 == ok) return (0) ;

        ok = base_url_Match_relative(relative, str) ;
        if (0 == ok) return (0) ;

        ok = base_url_Match_params(params, str) ;
        if (0 == ok) return (0) ;

        return 1 ;
}


/*
 *  API
 */

int base_url_parseURL ( /*OUT*/ base_url_t *urlbase, /*IN*/ char *urlstr )
{
        char *pch, *fch ;
        int   ok ;

        fch = pch = base_strdup(urlstr) ;
        if (NULL == pch)  {
            return -1;
	}

        ok  = base_url_Match_url( &(urlbase->protocol),
                                  &(urlbase->user),
                                  &(urlbase->machine),
                                  &(urlbase->port),
                                  &(urlbase->file),
                                  &(urlbase->relative),
                                  &(urlbase->params),
                                  &pch ) ;
        free(fch) ;
        return ok ;
}

int base_url_freeURL ( /* OUT */ base_url_t *urlbase )
{
        free(urlbase->protocol) ;
        free(urlbase->user) ;
        free(urlbase->machine) ;
        free(urlbase->file) ;
        free(urlbase->relative) ;
        free(urlbase->params) ;

        return 1 ;
}

// dup and free

base_url_t *base_url_dup ( base_url_t  *base_url )
{
	int ret ;
        base_url_t *elto ;
       
        ret = mfs_malloc((char **)&(elto), sizeof(base_url_t)) ;
	if (ret < 0) return NULL ;

	elto->protocol = base_strdup(base_url->protocol) ;
	elto->user     = base_strdup(base_url->user) ;
	elto->machine  = base_strdup(base_url->machine) ;
        elto->port     = base_url->port ;
	elto->file     = base_strdup(base_url->file) ;
	elto->relative = base_strdup(base_url->relative) ;
	elto->params   = base_strdup(base_url->params) ;

	// return new element
	return elto ;
}

int base_url_free ( base_url_t **base_url )
{
        mfs_free(&((*base_url)->protocol)) ;
        mfs_free(&((*base_url)->user)) ;
        mfs_free(&((*base_url)->machine)) ;
        mfs_free(&((*base_url)->file)) ;
        mfs_free(&((*base_url)->relative)) ;
        mfs_free(&((*base_url)->params)) ;

        mfs_free((char **)base_url) ;

	// return OK
	return 1 ;
}

