
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


int  mfs_dbm_gdbm::open  ( const char *path_name, int flags )
{
#ifdef HAVE_GDBM_H
     // Open file
     this->fd = ::gdbm_open((char *)path_name, 0, flags, 0755, NULL) ;
     if (NULL == this->fd) {
	 fprintf(stderr, "gdbm_open: %s\n", gdbm_strerror(gdbm_errno)) ;
         return -1 ;
     }
#endif

     this->dbm_backend = 1 ;
     this->dbm_backend_name = "GDBM" ;

     // Return OK
     return 1 ;
}

int   mfs_dbm_gdbm::close ( void )
{
#ifdef HAVE_GDBM_H
     // Close file
     if (NULL != this->fd) {
         ::gdbm_close(this->fd) ;
     }
#endif

     // Return OK
     return 1 ;
}

int   mfs_dbm_gdbm::store   ( void *buff_key, int count_key, void  *buff_val, int  count_val )
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
     ret = ::gdbm_store(this->fd, key, value, GDBM_REPLACE) ;
     if (ret != 0) {
	 fprintf(stderr, "gdbm_store: %s\n", gdbm_strerror(gdbm_errno)) ;
     }
#endif

     // Stats...
     (this->n_write_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_gdbm::fetch  ( void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int ret = -1 ;

#ifdef HAVE_GDBM_H
     datum key, value ;
     int cpy_size ;

     // Build key
     key.dptr  = (char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     value = ::gdbm_fetch(this->fd, key) ;

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
	     fprintf(stderr, "gdbm_fetch: %s\n", gdbm_db_strerror(this->fd)) ;
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

     // Stats...
     (this->n_read_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_gdbm::del  ( void *buff_key, int count_key )
{
     int ret = -1 ;

#ifdef HAVE_GDBM_H
     datum key ;

     // Build key
     key.dptr  = (char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     ret = ::gdbm_delete(this->fd, key) ;
     if (ret < 0) {
	 fprintf(stderr, "gdbm_delete: %s\n", gdbm_strerror(gdbm_errno)) ;
     }
#endif

     // Stats...
     (this->n_write_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_gdbm::stats_show ( char *prefix )
{
    // Print stats...
    printf("%s: File:\n",           prefix) ;
    printf("%s: + been_used=1\n",   prefix) ;
    printf("%s: + dbm_fd=%d\n",     prefix, this->fd) ;
    printf("%s: + protocol=%s\n",   prefix, this->dbm_backend_name) ;
    printf("%s: + # read=%ld\n",    prefix, this->n_read_req) ;
    printf("%s: + # write=%ld\n",   prefix, this->n_write_req) ;

    // Return OK
    return 1 ;
}


