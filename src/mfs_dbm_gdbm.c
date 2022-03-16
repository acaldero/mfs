
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
     // Check params...
     (*fd) = gdbm_open((char *)path_name, 0, flags, 0755, NULL) ;
     if (NULL == (*fd)) {
	 perror("gdbm_open: ") ;
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
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_gdbm_fetch  ( GDBM_FILE  fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
#ifdef HAVE_GDBM_H
     datum key, value ;
     int cpy_size ;

     // Build key
     key.dptr    = (char *)buff_key ;
     key.dsize   = count_key ;

     // Fetch key+val
     value = gdbm_fetch(fd, key) ;

     // Get val
     cpy_size = (*count_val < value.dsize) ? *count_val : value.dsize ;
     memcpy(buff_val, (void *)value.dptr, cpy_size) ;
     *count_val = cpy_size ;

     // Return OK/KO
     return (value.dptr != NULL) ;
#else
     // Return KO
     return -1 ;
#endif
}

int   mfs_dbm_gdbm_delete  ( GDBM_FILE  fd, void *buff_key, int count_key )
{
     int ret = -1 ;
#ifdef HAVE_GDBM_H
     datum key, value ;

     // Build key
     key.dptr  = (char *)buff_key ;
     key.dsize = count_key ;

     // Fetch key+val
     ret = gdbm_delete(fd, key) ;
#endif

     // Return OK/KO
     return ret ;
}

