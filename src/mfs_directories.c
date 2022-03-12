
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
 *  File System API: descriptors
 */

descriptor_t mfs_dir_des ;


long  mfs_directory_fd2long ( int fd )
{
    dir_t *dh ;

    // Get file handler
    dh = (dir_t *)mfs_descriptor_fd2fh(&mfs_dir_des, fd, sizeof(dir_t)) ;
    if (NULL == dh) {
	return -1 ;
    }

    // Return file descriptor
    return (long)fd ;
}

int mfs_directory_long2fd ( int *fd, long fref, int directory_backend )
{
    dir_t *dh ;

    // Get file handler
    dh = (dir_t *)mfs_descriptor_fd2fh(&mfs_dir_des, fref, sizeof(dir_t)) ;
    if (NULL == dh) {
	return -1 ;
    }

    // Get associated file descriptor (itself)
    (*fd) = fref ;

    // Return OK
    return 1 ;
}

int  mfs_directory_stats_show ( int fd, char *prefix )
{
    dir_t *dh ;

    // Get file handler
    dh = (dir_t *)mfs_descriptor_fd2fh(&mfs_dir_des, fd, sizeof(dir_t)) ;
    if (NULL == dh) {
	return -1 ;
    }

    // Print stats...
    printf("%s: Directory:\n",             prefix) ;
    printf("%s: + been_used=1\n",          prefix) ;
    printf("%s: + dir_descriptor=%d\n",    prefix, dh->dir_descriptor) ;
    printf("%s: + protocol=%s\n",          prefix, dh->directory_backend_name) ;
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

    // initialize descriptors
    ret = mfs_descriptor_init(&mfs_dir_des, 1024, sizeof(dir_t)) ;
    if (ret < 0) {
	return -1 ;
    }

    // initialize all protocols
    ret = mfs_directory_posix_init() ;
    if (ret < 0) {
	return -1 ;
    }

    ret = mfs_directory_redis_init() ;
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

    ret = mfs_directory_redis_finalize() ;
    if (ret < 0) {
	return -1 ;
    }

    // finalize descriptors
    ret = mfs_descriptor_finalize(&mfs_dir_des) ;
    if (ret < 0) {
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int  mfs_directory_opendir ( int *fd, int directory_backend, const char *path_name )
{
    int    ret ;
    dir_t *dh ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    (*fd) = ret = mfs_descriptor_find_free(&mfs_dir_des, sizeof(dir_t)) ;
    if (ret < 0) {
	return -1 ;
    }

    // Open directory
    dh = (dir_t *)mfs_descriptor_fd2fh(&mfs_dir_des, ret, sizeof(dir_t)) ;
    dh->directory_backend = directory_backend ;
    switch (dh->directory_backend)
    {
        case DIRECTORY_USE_POSIX:
             dh->directory_backend_name = "POSIX" ;
             ret = (long)mfs_directory_posix_opendir(&(dh->posix_fd), path_name) ;
             break ;

        case DIRECTORY_USE_REDIS:
             dh->directory_backend_name = "REDIS" ;
             ret = mfs_directory_redis_opendir(&(dh->redis_ctxt), &(dh->redis_key), path_name) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_backend(%d).\n", dh->directory_backend) ;
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
    dh = (dir_t *)mfs_descriptor_fd2fh(&mfs_dir_des, fd, sizeof(dir_t)) ;
    if (NULL == dh) {
	return -1 ;
    }

    // Close directory
    switch (dh->directory_backend)
    {
        case DIRECTORY_USE_POSIX:
             ret = mfs_directory_posix_closedir(dh->posix_fd) ;
             break ;

        case DIRECTORY_USE_REDIS:
             ret = mfs_directory_redis_closedir((dh->redis_ctxt), &(dh->redis_key)) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_backend(%d).\n", dh->directory_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    mfs_descriptor_set_free(&mfs_dir_des, fd) ;
    return ret ;
}

int   mfs_directory_readdir  ( int fd, struct dirent *dent )
{
    int ret = -1 ;
    dir_t *dh ;

    // Check params...
    dh = (dir_t *)mfs_descriptor_fd2fh(&mfs_dir_des, fd, sizeof(dir_t)) ;
    if (NULL == dh) {
	return -1 ;
    }

    // Read from directory...
    switch (dh->directory_backend)
    {
        case DIRECTORY_USE_POSIX:
             dent = mfs_directory_posix_readdir(dh->posix_fd) ;
	     if (NULL == dent) {
		 return -1 ;
	     }
             break ;

        case DIRECTORY_USE_REDIS:
             ret = mfs_directory_redis_readdir(dh->redis_ctxt, dh->redis_key, dent, sizeof(struct dirent)) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_backend(%d).\n", dh->directory_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int   mfs_directory_mkdir  ( int directory_backend, char *path_name, mode_t mode )
{
    int ret = -1 ;

    // Read from directory...
    switch (directory_backend)
    {
        case DIRECTORY_USE_POSIX:
             ret = mfs_directory_posix_mkdir(path_name, mode) ;
             break ;

        case DIRECTORY_USE_REDIS:
	     // TODO:
             // ret = mfs_directory_redis_mkdir(fh->redis_ctxt, fh->redis_key, path_name, strlen(path_name)+1) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_backend(%d).\n", directory_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int   mfs_directory_rmdir  ( int directory_backend, char *path_name )
{
    int ret = -1 ;

    // Read from directory...
    switch (directory_backend)
    {
        case DIRECTORY_USE_POSIX:
             ret = mfs_directory_posix_rmdir(path_name) ;
             break ;

        case DIRECTORY_USE_REDIS:
	     // TODO:
             // ret = mfs_directory_redis_rmdir(fh->redis_ctxt, fh->redis_key, path_name, strlen(path_name)+1) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on directory_backend(%d).\n", directory_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

