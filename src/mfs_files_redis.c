
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


#include "mfs_files_redis.h"


/*
 *  File System API
 */

int  mfs_file_redis_init ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_file_redis_finalize ( void )
{
    // Return OK
    return 1 ;
}

int  mfs_file_redis_open  ( redisContext **red_ctxt, char **red_key, const char *path_name )
{
     int  ret = -1 ;

#ifdef HAVE_HIREDIS_H
     char *ip4_addr = "127.0.0.1" ;
     int   ip4_port = 6379 ;

     // connect
     (*red_ctxt) = redisConnect(ip4_addr, ip4_port)  ;
     if (NULL == (*red_ctxt)) {
	 mfs_print(DBG_INFO, "[FILE]: ERROR on redisConnect('%s', %d).\n", ip4_addr, ip4_port) ;
	 return -1 ;
     }
     // set key
     (*red_key) = strdup(path_name) ;
#endif

     // Return file descriptor
     return ret ;
}

int  mfs_file_redis_close ( redisContext  *red_ctxt, char **red_key )
{
     // Close file
#ifdef HAVE_HIREDIS_H
     redisFree(red_ctxt) ;
     free((*red_key)) ;
     (*red_key) = NULL ;
#endif

     // Return OK/KO
     return 1 ;
}

int  mfs_file_redis_read   ( redisContext  *red_ctxt, char  *red_key, long *offset, void *buffer, int buffer_size )
{
    long to_read = buffer_size ;
#ifdef HAVE_HIREDIS_H
    redisReply *reply = NULL ;

    reply = (redisReply *)redisCommand(red_ctxt, "GET %b", red_key, (size_t)strlen(red_key)) ;
    if (reply->len < ((*offset) + buffer_size)) {
	to_read = reply->len - (*offset) ;
    }

    memmove(buffer, reply->str + (*offset), to_read) ;
    (*offset) = (*offset) + to_read ;
    freeReplyObject(reply) ;
#endif

    // Return OK
    return to_read ;
}

int  mfs_file_redis_write  ( redisContext  *red_ctxt, char  *red_key, long *offset, void *buffer, int buffer_size )
{
#ifdef HAVE_HIREDIS_H
    redisReply *reply = NULL ;

    reply = (redisReply *)redisCommand(red_ctxt, "APPEND %b %b", red_key, (size_t)strlen(red_key), buffer, (size_t)buffer_size) ;
    (*offset) = (*offset) + buffer_size ;
    freeReplyObject(reply) ;
#endif

    // Return OK
    return buffer_size ;
}

