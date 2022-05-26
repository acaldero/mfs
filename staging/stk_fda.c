
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


#include "stk_fda.h"


/*
 *  API
 */

int   stk_fda_init ( stk_fda_t *des, int neltos, int elto_size )
{
    int ret ;
    int m_size ;

    des->hash_neltos = neltos ;
    des->mutex       = PTHREAD_MUTEX_INITIALIZER ;

    // malloc hash_eltos, been_used and file_fd
    ret = mfs_malloc((char **)&(des->hash_eltos), elto_size * (des->hash_neltos)) ;
    if (ret < 0) {
        return -1 ;
    }

    ret = mfs_malloc((char **)&(des->been_used), elto_size * sizeof(int)) ;
    if (ret < 0) {
        return -1 ;
    }

    ret = mfs_malloc((char **)&(des->file_fd),   elto_size * sizeof(int)) ;
    if (ret < 0) {
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int   stk_fda_finalize ( stk_fda_t *des )
{
    if (NULL == des)  {
	return 1 ;
    }

    // free hash_eltos, been_used and file_fd
    mfs_free((char **)&(des->hash_eltos)) ;
    mfs_free((char **)&(des->been_used)) ;
    mfs_free((char **)&(des->file_fd)) ;

    // Return OK
    return 1 ;
}

int   stk_fda_find_free ( stk_fda_t *des, int elto_size )
{
    pthread_mutex_lock(&(des->mutex)) ;
    for (int i=0; i<des->hash_neltos; i++)
    {
	 if (0 == des->been_used[i])
	 {
              memset((i*elto_size)+(des->hash_eltos), 0, elto_size) ;
	      des->been_used[i] = 1 ;
	      des->file_fd[i]   = i ;
             pthread_mutex_unlock(&(des->mutex)) ;
	     return i ;
	 }
    }

    pthread_mutex_unlock(&(des->mutex)) ;
    return -1 ;
}

void  stk_fda_set_free ( stk_fda_t *des, int fd )
{
    pthread_mutex_lock(&(des->mutex)) ;
     des->been_used[fd] =  0 ;
     des->file_fd[fd]   = -1 ;
    pthread_mutex_unlock(&(des->mutex)) ;
}

int   stk_fda_is_free ( stk_fda_t *des, int fd )
{
    int been_used ;

    pthread_mutex_lock(&(des->mutex)) ;
    been_used = des->been_used[fd] ;
    pthread_mutex_unlock(&(des->mutex)) ;

    return (0 == been_used) ;
}

void *stk_fda_fd2fh    ( stk_fda_t *des, int fd, int elto_size )
{
    void *fh ;

    // Check params...
    if ( (fd < 0) || (fd >= des->hash_neltos) ) {
	  return NULL ;
    }
    if (des->been_used[fd] != 1) {
	return NULL ;
    }

    // Get associated file handler
    fh = (void *)((char *)(des->hash_eltos) + fd*elto_size) ;

    // Return File Hanlder
    return fh ;
}

