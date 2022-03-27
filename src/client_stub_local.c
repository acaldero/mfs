
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


#include "client_stub_local.h"


/*
 *  File System API
 */

int clientstub_local_init ( comm_t *wb, params_t *params, conf_t *conf )
{
    int ret = 0 ;

    // Initialize files, dbm and directories
    if (ret >= 0) {
	ret = mfs_file_init() ;
    }

    if (ret >= 0) {
	ret = mfs_dbm_init() ;
    }

    if (ret >= 0) {
	ret = mfs_directory_init() ;
    }

    // Return OK/KO
    return ret ;
}

int clientstub_local_finalize ( comm_t *wb, params_t *params )
{
    int ret = 0 ;

    // Finalize files, dbm and directories
    if (ret >= 0) {
	ret = mfs_directory_finalize() ;
    }

    if (ret >= 0) {
	ret = mfs_file_finalize() ;
    }

    if (ret >= 0) {
	ret = mfs_dbm_finalize() ;
    }

    // Return OK/KO
    return ret ;
}


/*
 *  File API
 */

long clientstub_local_open ( comm_t *wb, const char *pathname, int flags )
{
    int ret = 0 ;
    int fd ;

    ret = mfs_file_open(&fd, FILE_USE_POSIX, pathname, flags) ;
    if (ret < 0) {
	mfs_print(DBG_WARNING, "[CLIENTSTUB_LOCAL]: file '%s' not opened :-(\n", pathname) ;
	return -1 ;
    }

    return fd ;
}

int  clientstub_local_close ( comm_t *wb, long fd )
{
     return mfs_file_close(fd) ;
}

int  clientstub_local_read ( comm_t *wb, long fd, void *buff_char, int count )
{
     return mfs_file_read(fd, buff_char, count) ;
}

int  clientstub_local_write ( comm_t *wb, long fd, void *buff_char, int count )
{
     return mfs_file_write(fd, buff_char, count) ;
}


/*
 *  Directory API
 */

long clientstub_local_mkdir ( comm_t *wb, const char *pathname, int mode )
{
     return mfs_directory_mkdir(DIRECTORY_USE_POSIX, (char *)pathname, mode) ;
}

long clientstub_local_rmdir ( comm_t *wb, const char *pathname )
{
     return mfs_directory_rmdir(DIRECTORY_USE_POSIX, (char *)pathname) ;
}


/*
 *  DBM File API
 */

long clientstub_local_dbmopen ( comm_t *wb, const char *pathname, int flags )
{
     int ret ;
     int fd ;

     ret = mfs_dbm_open(&fd, DBM_USE_GDBM, pathname, flags) ;
     if (ret < 0) {
	 mfs_print(DBG_WARNING, "[CLIENTSTUB_LOCAL]: file '%s' not opened :-(\n", pathname) ;
	 return -1 ;
     }

     return fd ;
}

int  clientstub_local_dbmclose ( comm_t *wb, long fd )
{
     return mfs_dbm_close(fd) ;
}

int  clientstub_local_dbmstore ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
     return mfs_dbm_store(fd, buff_key, count_key, buff_val, count_val) ;
}

int  clientstub_local_dbmfetch ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     return mfs_dbm_fetch(fd, buff_key, count_key, &buff_val, count_val) ;
}

int  clientstub_local_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key )
{
     return mfs_dbm_delete(fd, buff_key, count_key) ;
}

