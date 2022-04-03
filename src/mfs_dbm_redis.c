
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


#include "mfs_dbm_redis.h"


/*
 *  File System API
 */

int  mfs_dbm_redis_init ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_dbm_redis_finalize ( void )
{
    // Return OK
    return 1 ;
}


int  mfs_dbm_redis_open  ( redisContext  **fd, const char *path_name, int flags )
{
#ifdef HAVE_HIREDIS_H
     // Close connection
     (*fd) = redisConnect(path_name, flags) ;
     if ((*fd) != NULL && (*fd)->err) {
         printf("[DBM_REDIS] Error: %s\n", (*fd)->errstr);
         return -1 ;
     }
#endif

     // Return OK
     return 1 ;
}

int   mfs_dbm_redis_close ( redisContext *fd )
{
#ifdef HAVE_HIREDIS_H
     // Close connection
     if (NULL != fd) {
	 redisFree(fd) ;
     }
#endif

     // Return OK
     return 1 ;
}

int   mfs_dbm_redis_store   ( redisContext *fd, void *buff_key, int count_key, void  *buff_val, int  count_val )
{
     int ret = -1 ;
#ifdef HAVE_HIREDIS_H
     redisReply *reply ;

     // Store key+val
     reply = (redisReply *)redisCommand(fd, "SET %s %s", buff_key, buff_val) ;
     freeReplyObject(reply) ;
     ret = 1 ;
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_redis_fetch  ( redisContext *fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int ret = -1 ;
#ifdef HAVE_HIREDIS_H
     redisReply *reply ;

     // Fetch key+val
     reply = (redisReply *)redisCommand(fd, "GET %s", buff_key) ;
     printf("%s\n", reply->str) ;
     strcpy((char *)buff_val, reply->str) ;
     freeReplyObject(reply) ;
     ret = 1 ;
#endif

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_redis_delete  ( redisContext *fd, void *buff_key, int count_key )
{
     int ret = -1 ;
#ifdef HAVE_HIREDIS_H
     redisReply *reply ;

     // Delete key+val
     reply = (redisReply *)redisCommand(fd, "DEL %s", buff_key) ;
     freeReplyObject(reply) ;
     ret = 1 ;
#endif

     // Return OK/KO
     return ret ;
}

