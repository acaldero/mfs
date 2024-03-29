
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


#include "mfs_dbm_tdb.h"


/*
 *  File System API
 */

int  mfs_dbm_tdb_init ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_dbm_tdb_finalize ( void )
{
    // Return OK
    return 1 ;
}


int  mfs_dbm_tdb_open  ( TDB_CONTEXT **fd, const char *path_name, int flags )
{
#ifdef HAVE_TDB_H
     int hash_size = 0 ;
     int tdb_flags = 0 ; // TDB_DEFAULT ;

     // Open file
     (*fd) = tdb_open((char *)path_name, hash_size, tdb_flags, flags, 0700) ;
     if (NULL == (*fd)) {
	 perror("tdb_open: ") ;
         return -1 ;
     }
#endif

     // Return OK
     return 1 ;
}

int   mfs_dbm_tdb_close ( TDB_CONTEXT *fd )
{
     int ret = -1 ;

#ifdef HAVE_TDB_H
     // Close file
     if (NULL != fd) {
         ret = tdb_close(fd) ;
         if (ret != 0) {
	     fprintf(stderr, "tdb_close fails :-S\n") ;
         }
     }
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_tdb_store   ( TDB_CONTEXT *fd, void *buff_key, int count_key, void  *buff_val, int  count_val )
{
     int ret = -1 ;

#ifdef HAVE_TDB_H
     TDB_DATA key, value ;

     // Build key+val
     key.dptr    = (unsigned char *)buff_key ;
     key.dsize   = count_key ;
     value.dptr  = (unsigned char *)buff_val ;
     value.dsize = count_val ;

     // Store key+val
     ret = tdb_store(fd, key, value, TDB_REPLACE) ;
     if (ret != 0) {
	 fprintf(stderr, "tdb_store: fails :-S\n") ;
     }
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_tdb_fetch  ( TDB_CONTEXT *fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int ret = -1 ;

#ifdef HAVE_TDB_H
     TDB_DATA key, value ;
     int cpy_size ;

     // Build key
     key.dptr  = (unsigned char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     value = tdb_fetch(fd, key) ;

     // Check returned value
     if (value.dptr == NULL) {
	 fprintf(stderr, "tdb_fetch fails :-S\n") ;
         return -1 ;
     }

     // Copy value to buffer_val and free memory
     cpy_size = (*count_val < value.dsize) ? *count_val : value.dsize ;
     memcpy(buff_val, (void *)value.dptr, cpy_size) ;
     *count_val = cpy_size ;
     free(value.dptr) ;
     ret = 1 ;
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_tdb_delete  ( TDB_CONTEXT *fd, void *buff_key, int count_key )
{
     int ret = -1 ;

#ifdef HAVE_TDB_H
     TDB_DATA key ;

     // Build key
     key.dptr  = (unsigned char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     ret = tdb_delete(fd, key) ;
     if (ret < 0) {
	 fprintf(stderr, "tdb_delete fails :-S\n") ;
     }
#endif

     // Return OK/KO
     return ret ;
}

