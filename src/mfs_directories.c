
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


#include "mfs_directories.h"


/*
 *  Auxiliar functions
 */

            int mfs_directory_hash_neltos = 1024 ;
          dir_t mfs_directory_hash_eltos[1024] ;
pthread_mutex_t mfs_directory_mutex = PTHREAD_MUTEX_INITIALIZER ;

int mfs_directory_find_free ( void )
{
    pthread_mutex_lock(&mfs_directory_mutex) ;
    for (int i=0; i<mfs_directory_hash_neltos; i++)
    {
	 if (0 == mfs_directory_hash_eltos[i].been_used)
	 {
             memset(&(mfs_directory_hash_eltos[i]), 0, sizeof(dir_t)) ;
	     mfs_directory_hash_eltos[i].been_used      = 1 ;
	     mfs_directory_hash_eltos[i].dir_descriptor = i ;
             pthread_mutex_unlock(&mfs_directory_mutex) ;
	     return i ;
	 }
    }

    pthread_mutex_unlock(&mfs_directory_mutex) ;
    return -1 ;
}

void mfs_directory_set_free ( int fd )
{
    pthread_mutex_lock(&mfs_directory_mutex) ;
     mfs_directory_hash_eltos[fd].been_used      = 0 ;
     mfs_directory_hash_eltos[fd].dir_descriptor = -1 ;
    pthread_mutex_unlock(&mfs_directory_mutex) ;
}


/*
 *  File System API: descriptors
 */

long  mfs_directory_fd2long ( int fd )
{
    dir_t *dh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_directory_hash_neltos) ) {
	  return -1 ;
    }

    // Return directory descriptor
    dh = &(mfs_directory_hash_eltos[fd]) ;
    return dh->dir_descriptor ;
}

int mfs_directory_long2fd ( int *fd, long fref, int directory_protocol )
{
    // Check params...
    if ( (fref < 0) || (fref >= mfs_directory_hash_neltos) ) {
	  return -1 ;
    }

    // Return OK
    (*fd) = fref ;
    return 1 ;
}

int  mfs_directory_stats_show ( int fd, char *prefix )
{
    dir_t *dh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_directory_hash_neltos) ) {
	  return -1 ;
    }

    dh = &(mfs_directory_hash_eltos[fd]) ;
    if (dh->been_used != 1) {
	return -1 ;
    }

    // Print stats...
    printf("%s: Directory:\n",             prefix) ;
    printf("%s: + been_used=%d\n",         prefix, dh->been_used) ;
    printf("%s: + dir_descriptor=%d\n",    prefix, dh->dir_descriptor) ;
    printf("%s: + protocol=%s\n",          prefix, dh->directory_protocol_name) ;
    printf("%s:   + posix_fd=%d\n",        prefix, dh->posix_fd) ;

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int  mfs_directory_init ( void )
{
    int  ret ;

    // initialize all protocols
    ret = mfs_directory_posix_init() ;
    if (ret < 0) {
	return -1 ;
    }

    ret = mfs_directory_red_init() ;
    if (ret < 0) {
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int  mfs_directory_finalize ( void )
{
    int  ret ;

    // finalize all protocols
    ret = mfs_directory_posix_finalize() ;
    if (ret < 0) {
	return -1 ;
    }

    ret = mfs_directory_red_finalize() ;
    if (ret < 0) {
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int  mfs_directory_opendir ( int *fd, int directory_protocol, const char *path_name )
{
    int    ret ;
    dir_t *dh ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    (*fd) = ret = mfs_directory_find_free() ;
    if (ret < 0) {
	return -1 ;
    }

    // Open directory
    dh = &(mfs_directory_hash_eltos[ret]) ;
    dh->directory_protocol = directory_protocol ;
    switch (dh->directory_protocol)
    {
        case DIRECTORY_USE_POSIX:
             dh->directory_protocol_name = "POSIX" ;
             ret = (long)mfs_directory_posix_opendir(&(dh->posix_fd), path_name) ;
             break ;

        case DIRECTORY_USE_REDIS:
             dh->directory_protocol_name = "REDIS" ;
             ret = mfs_directory_red_opendir(&(dh->redis_ctxt), &(dh->redis_key), path_name) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_protocol(%d).\n", dh->directory_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int   mfs_directory_closedir ( int  fd )
{
    int    ret ;
    dir_t *dh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_directory_hash_neltos) ) {
	  return -1 ;
    }

    dh = &(mfs_directory_hash_eltos[fd]) ;
    if (dh->been_used != 1) {
	return -1 ;
    }

    // Close directory
    switch (dh->directory_protocol)
    {
        case DIRECTORY_USE_POSIX:
             ret = mfs_directory_posix_closedir(dh->posix_fd) ;
             break ;

        case DIRECTORY_USE_REDIS:
             ret = mfs_directory_red_closedir((dh->redis_ctxt), &(dh->redis_key)) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_protocol(%d).\n", dh->directory_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    mfs_directory_set_free(fd) ;
    return ret ;
}

int   mfs_directory_readdir  ( int fd, struct dirent *dent )
{
    int ret = -1 ;
    dir_t *dh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_directory_hash_neltos) ) {
	  return -1 ;
    }

    dh = &(mfs_directory_hash_eltos[fd]) ;
    if (dh->been_used != 1) {
	return -1 ;
    }

    // Read from directory...
    switch (dh->directory_protocol)
    {
        case DIRECTORY_USE_POSIX:
             dent = mfs_directory_posix_readdir(dh->posix_fd) ;
	     if (NULL == dent) {
		 return -1 ;
	     }
             break ;

        case DIRECTORY_USE_REDIS:
             ret = mfs_directory_red_readdir(dh->redis_ctxt, dh->redis_key, dent, sizeof(struct dirent)) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_protocol(%d).\n", dh->directory_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int   mfs_directory_mkdir  ( int directory_protocol, char *path_name, mode_t mode )
{
    int ret = -1 ;

    // Read from directory...
    switch (directory_protocol)
    {
        case DIRECTORY_USE_POSIX:
             ret = mfs_directory_posix_mkdir(path_name, mode) ;
             break ;

        case DIRECTORY_USE_REDIS:
	     // TODO:
             // ret = mfs_directory_red_mkdir(fh->redis_ctxt, fh->redis_key, path_name, strlen(path_name)+1) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_protocol(%d).\n", directory_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int   mfs_directory_rmdir  ( int directory_protocol, char *path_name )
{
    int ret = -1 ;

    // Read from directory...
    switch (directory_protocol)
    {
        case DIRECTORY_USE_POSIX:
             ret = mfs_directory_posix_rmdir(path_name) ;
             break ;

        case DIRECTORY_USE_REDIS:
	     // TODO:
             // ret = mfs_directory_red_rmdir(fh->redis_ctxt, fh->redis_key, path_name, strlen(path_name)+1) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_protocol(%d).\n", directory_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

