
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


#include "mfs_client_api.h"


/*
 *  File System API
 */

int mfs_api_init ( comm_t *wb, params_t *params )
{
    return clientstub_init(wb, params) ;
}

int mfs_api_finalize ( comm_t *wb, params_t *params )
{
    return clientstub_finalize(wb, params) ;
}


/*
 *  File API
 */

long mfs_api_open ( comm_t *wb, const char *pathname, int flags )
{
     return clientstub_open(wb, pathname, flags) ;
}

int  mfs_api_close ( comm_t *wb, long fd )
{
     return clientstub_close(wb, fd) ;
}

int mfs_api_read ( comm_t *wb, long fd, void *buff_char, int count )
{
     return clientstub_read(wb, fd, buff_char, count) ;
}

int mfs_api_write ( comm_t *wb, long fd, void *buff_char, int count )
{
     return clientstub_write(wb, fd, buff_char, count) ;
}


/*
 *  Directory API
 */

long mfs_api_mkdir ( comm_t *wb, const char *pathname, int mode )
{
     return clientstub_mkdir(wb, pathname, mode) ;
}

long mfs_api_rmdir ( comm_t *wb, const char *pathname )
{
     return clientstub_rmdir(wb, pathname) ;
}


/*
 *  DBM File API
 */

long mfs_api_dbmopen ( comm_t *wb, const char *pathname, int flags )
{
     return clientstub_dbmopen(wb, pathname, flags) ;
}

int  mfs_api_dbmclose ( comm_t *wb, long fd )
{
     return clientstub_dbmclose(wb, fd) ;
}

int  mfs_api_dbmstore ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
     return clientstub_dbmstore(wb, fd, buff_key, count_key, buff_val, count_val) ;
}

int  mfs_api_dbmfetch ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     return clientstub_dbmfetch(wb, fd, buff_key, count_key, buff_val, count_val) ;
}

int  mfs_api_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key )
{
     return clientstub_dbmdelete(wb, fd, buff_key, count_key) ;
}

