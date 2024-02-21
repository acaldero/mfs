
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


#include "mfs_directories_posix.h"


/*
 *  File System API
 */

int  mfs_directory_posix::opendir ( const char *path_name )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(path_name, "[DIR_POSIX] NULL path_name :-/", -1) ;

    // Open file
    this->fd = opendir(path_name) ;
    if (NULL == this->) {
    	mfs_print(DBG_INFO, "[DIR]: ERROR on opendir(path_name='%s')\n", path_name) ;
        return -1 ;
    }

    // Return OK
    return 1 ;
}

int   mfs_directory_posix::closedir ( void )
{
    int ret = -1 ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(this->fd, "[DIR_POSIX] NULL fd :-/", -1) ;

    // Close file
    ret = closedir(this->fd) ;
    if (ret < 0) {
    	mfs_print(DBG_INFO, "[DIR]: ERROR on closedir(fd='%s')\n", fd) ;
        return -1 ;
    }

    this->fd = NULL ;

    // Return OK
    return 1 ;
}

struct dirent *mfs_directory_posix::readdir  ( void )
{
    struct dirent *ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(this->fd, "[DIR_POSIX] NULL fd :-/", NULL) ;

    // Read from file...
    ret = readdir(this->fd) ;
    if (NULL == ret) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on read entry from directory '%d'\n", fd) ;
	return NULL ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_directory_posix::mkdir  ( char *path_name, mode_t mode )
{
    int ret = -1 ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(path_name, "[DIR_POSIX] NULL path_name :-/", -1) ;

    // mkdir directory...
    ret = mkdir(path_name, mode) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on mkdir directory '%s'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_directory_posix::rmdir  ( char *path_name )
{
    int ret = -1 ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(path_name, "[DIR_POSIX] NULL path_name :-/", -1) ;

    // Read from file...
    ret = rmdir(path_name) ;
    if (ret < 0) {
	mfs_print(DBG_INFO, "[DIR]: ERROR on rmdir directory '%s'\n", path_name) ;
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}


