
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MFS.
 *
 *  MFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "mfs_directories_posix.h"


/*
 *  File System API
 */

int  mfs_directory_posix_init ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_directory_posix_finalize ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_directory_posix_opendir ( DIR **fd, const char *path_name )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Open file
    (*fd) = opendir(path_name) ;
    if (NULL == (*fd)) {
    	mfs_print(DBG_INFO, "[DIR]: ERROR on opendir(path_name='%s')\n", path_name) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int   mfs_directory_posix_closedir ( DIR *fd )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Close file
    ret = closedir(fd) ;
    if (ret < 0) {
    	mfs_print(DBG_INFO, "[DIR]: ERROR on closedir(fd='%s')\n", fd) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

struct dirent *mfs_directory_posix_readdir  ( DIR *fd )
{
    struct dirent *ret ;

    // Check params...
    if (NULL == fd) {
	return NULL ;
    }

    // Read from file...
    ret = readdir(fd) ;
    if (NULL == ret) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on read entry from directory '%d'\n", fd) ;
	return NULL ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_directory_posix_mkdir  ( char *path_name, mode_t mode )
{
    int ret = -1 ;

    // mkdir directory...
    ret = mkdir(path_name, mode) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on mkdir directory '%s'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_directory_posix_rmdir  ( char *path_name )
{
    int ret = -1 ;

    // Read from file...
    ret = rmdir(path_name) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on rmdir directory '%s'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

