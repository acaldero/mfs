
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
 *  File System API
 */

long  mfs_directory_fd2long ( dir_t *fd )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    if (DIRECTORY_USE_POSIX  == fd->file_protocol) {
        return (long) fd->posix_fd ;
    }

    // Return KO
    return -1 ;
}

int mfs_directory_long2fd ( dir_t *fd, long fref, int file_protocol )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    fd->file_protocol = file_protocol ;
    fd->posix_fd = 0 ;

    if (DIRECTORY_USE_POSIX  == fd->file_protocol) {
        fd->posix_fd = (DIR *)fref ;
    }

    // Return OK
    return 1 ;
}

int  mfs_directory_stats_show ( dir_t *fd, char *prefix )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    char *proto = "unknow" ;
    if (DIRECTORY_USE_POSIX == fd->file_protocol)
        proto = "POSIX" ;

    // Print stats...
    printf("%s: Directory:\n",      prefix) ;
    printf("%s: + protocol=%s\n",   prefix, proto) ;
    printf("%s: + posix_fd=%d\n",   prefix, fd->posix_fd) ;

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int  mfs_directory_opendir ( dir_t *fd, int file_protocol, const char *path_name )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    fd->file_protocol = file_protocol ;
    fd->posix_fd      = 0 ;

    // Open file
    fd->posix_fd = opendir(path_name) ;
    if (NULL == fd->posix_fd) {
    	mfs_print(DBG_INFO, "[DIR]: ERROR on opendir(path_name='%s')\n", path_name) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int   mfs_directory_closedir ( dir_t *fd )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Close file
    ret = closedir(fd->posix_fd) ;

    // Return OK/KO
    return ret ;
}

struct dirent *mfs_directory_readdir  ( dir_t *fd )
{
    struct dirent *ret ;

    // Check params...
    if (NULL == fd) {
	return NULL ;
    }

    // Read from file...
    ret = readdir(fd->posix_fd) ;
    if (NULL == ret) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on read entry from directory '%d'\n", fd->posix_fd) ;
	return NULL ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_directory_mkdir  ( char *path_name, mode_t mode )
{
    int ret = -1 ;

    // mkdir directory...
    ret = mkdir(path_name, mode) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on mkdir directory '%d'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_directory_rmdir  ( char *path_name )
{
    int ret = -1 ;

    // Read from file...
    ret = rmdir(path_name) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on rmdir directory '%d'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

