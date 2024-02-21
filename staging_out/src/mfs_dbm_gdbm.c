
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


#include "mfs_dbm_gdbm.h"


/*
 *  File System API
 */

int  mfs_dbm_gdbm_init ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_dbm_gdbm_finalize ( void )
{
    // Return OK
    return 1 ;
}


int  mfs_dbm_gdbm_open  ( GDBM_FILE  *fd, const char *path_name, int flags )
{
#ifdef HAVE_GDBM_H
     // Open file
     (*fd) = gdbm_open((char *)path_name, 0, flags, 0755, NULL) ;
     if (NULL == (*fd)) {
	 fprintf(stderr, "gdbm_open: %s\n", gdbm_strerror(gdbm_errno)) ;
         return -1 ;
     }
#endif

     // Return OK
     return 1 ;
}

int   mfs_dbm_gdbm_close ( GDBM_FILE  fd )
{
#ifdef HAVE_GDBM_H
     // Close file
     if (NULL != fd) {
         gdbm_close(fd) ;
     }
#endif

     // Return OK
     return 1 ;
}

int   mfs_dbm_gdbm_store   ( GDBM_FILE  fd, void *buff_key, int count_key, void  *buff_val, int  count_val )
{
     int ret = -1 ;
#ifdef HAVE_GDBM_H
     datum key, value ;

     // Build key+val
     key.dptr    = (char *)buff_key ;
     key.dsize   = count_key ;
     value.dptr  = (char *)buff_val ;
     value.dsize = count_val ;

     // Store key+val
     ret = gdbm_store(fd, key, value, GDBM_REPLACE) ;
     if (ret != 0) {
	 fprintf(stderr, "gdbm_store: %s\n", gdbm_strerror(gdbm_errno)) ;
     }
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_gdbm_fetch  ( GDBM_FILE  fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int ret = -1 ;

#ifdef HAVE_GDBM_H
     datum key, value ;
     int cpy_size ;

     // Build key
     key.dptr  = (char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     value = gdbm_fetch(fd, key) ;

     // Check returned value
     if (value.dptr == NULL)
     {
         if (gdbm_errno == GDBM_ITEM_NOT_FOUND)
	 {
             fprintf(stderr, "gdbm_fetch: key not found\n") ;
             return -2 ;
         }
         else
	 {
	     fprintf(stderr, "gdbm_fetch: %s\n", gdbm_db_strerror(fd)) ;
             return -1 ;
         }
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

int   mfs_dbm_gdbm_delete  ( GDBM_FILE  fd, void *buff_key, int count_key )
{
     int ret = -1 ;

#ifdef HAVE_GDBM_H
     datum key ;

     // Build key
     key.dptr  = (char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     ret = gdbm_delete(fd, key) ;
     if (ret < 0) {
	 fprintf(stderr, "gdbm_delete: %s\n", gdbm_strerror(gdbm_errno)) ;
     }
#endif

     // Return OK/KO
     return ret ;
}

