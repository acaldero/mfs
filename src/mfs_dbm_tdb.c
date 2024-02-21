
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


#include "mfs_dbm_tdb.h"


/*
 *  File System API
 */


int  mfs_dbm_tdb::open  ( const char *path_name, int flags )
{
#ifdef HAVE_TDB_H
     int hash_size = 0 ;
     int tdb_flags = 0 ; // TDB_DEFAULT ;

     // Open file
     this->fd = tdb_open((char *)path_name, hash_size, tdb_flags, flags, 0700) ;
     if (NULL == this->fd) {
	 perror("tdb_open: ") ;
         return -1 ;
     }
#endif

     this->dbm_backend = 3 ;
     this->dbm_backend_name = "TDB" ;

     // Return OK
     return 1 ;
}

int   mfs_dbm_tdb::close ( void )
{
     int ret = -1 ;

#ifdef HAVE_TDB_H
     // Close file
     if (NULL != this->fd) {
         ret = tdb_close(this->fd) ;
         if (ret != 0) {
	     fprintf(stderr, "tdb_close fails :-S\n") ;
         }
     }
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_tdb::store   ( void *buff_key, int count_key, void  *buff_val, int  count_val )
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
     ret = tdb_store(this->fd, key, value, TDB_REPLACE) ;
     if (ret != 0) {
	 fprintf(stderr, "tdb_store: fails :-S\n") ;
     }
#endif

     // Stats...
     (this->n_write_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_tdb::fetch  ( void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int ret = -1 ;

#ifdef HAVE_TDB_H
     TDB_DATA key, value ;
     int cpy_size ;

     // Build key
     key.dptr  = (unsigned char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     value = tdb_fetch(this->fd, key) ;

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

     // Stats...
     (this->n_read_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_tdb::del     ( void *buff_key, int count_key )
{
     int ret = -1 ;

#ifdef HAVE_TDB_H
     TDB_DATA key ;

     // Build key
     key.dptr  = (unsigned char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     ret = tdb_delete(this->fd, key) ;
     if (ret < 0) {
	 fprintf(stderr, "tdb_delete fails :-S\n") ;
     }
#endif

     // Stats...
     (this->n_write_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_tdb::stats_show ( char *prefix )
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


