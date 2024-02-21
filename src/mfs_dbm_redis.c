
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


int  mfs_dbm_redis::open  ( const char *path_name, int flags )
{
#ifdef HAVE_HIREDIS_H
     // Close connection
     this->fd = ::redisConnect(path_name, flags) ;
     if (this->fd != NULL && this->fd->err) {
         printf("[DBM_REDIS] Error: %s\n", this->fd->errstr);
         return -1 ;
     }
#endif

     this->dbm_backend = 2 ;
     this->dbm_backend_name = "REDIS" ;

     // Return OK
     return 1 ;
}

int   mfs_dbm_redis::close ( void )
{
#ifdef HAVE_HIREDIS_H
     // Close connection
     if (NULL != this->fd) {
	 ::redisFree(this->fd) ;
     }
#endif

     // Return OK
     return 1 ;
}

int   mfs_dbm_redis::store   ( void *buff_key, int count_key, void  *buff_val, int  count_val )
{
     int ret = -1 ;
#ifdef HAVE_HIREDIS_H
     redisReply *reply ;

     // Store key+val
     reply = (redisReply *)redisCommand(this->fd, "SET %s %s", buff_key, buff_val) ;
     freeReplyObject(reply) ;
     ret = 1 ;
#endif

     // Stats...
     (this->n_write_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_redis::fetch  ( void *buff_key, int count_key, void *buff_val, int *count_val )
{
     int ret = -1 ;
#ifdef HAVE_HIREDIS_H
     redisReply *reply ;

     // Fetch key+val
     reply = (redisReply *)redisCommand(this->fd, "GET %s", buff_key) ;
     printf("%s\n", reply->str) ;
     strcpy((char *)buff_val, reply->str) ;
     freeReplyObject(reply) ;
     ret = 1 ;
#endif

     // Stats...
     (this->n_read_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_redis::del     ( void *buff_key, int count_key )
{
     int ret = -1 ;
#ifdef HAVE_HIREDIS_H
     redisReply *reply ;

     // Delete key+val
     reply = (redisReply *)redisCommand(this->fd, "DEL %s", buff_key) ;
     freeReplyObject(reply) ;
     ret = 1 ;
#endif

     // Stats...
     (this->n_write_req) ++ ;

     // Return OK/KO
     return ret ;
}

int   mfs_dbm_redis::stats_show ( char *prefix )
{
    // Print stats...
    printf("%s: File:\n",           prefix) ;
    printf("%s: + been_used=1\n",   prefix) ;
    printf("%s: + dbm_fd=%d\n",     prefix, this->fd) ;
    printf("%s: + protocol=%s\n",   prefix, this->dbm_backend_name) ;
    printf("%s: + # read=%ld\n",    prefix, this->n_read_req) ;
    printf("%s: + # write=%ld\n",   prefix, this->n_write_req) ;

    // Return OK
    return 1 ;
}


