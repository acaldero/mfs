
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

#ifndef __BASE_LIB_H__
#define __BASE_LIB_H__

    // Includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>

    #include <sys/time.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <pthread.h>

    // Consts
    #define ONE_KB                   (1024)
    #define ONE_MB            (1024 * 1024)
    #define ONE_GB     (1024 * 1024 * 1024)

    // debug: levels
    #define DBG_ERROR    1, __FILE__, __LINE__, stderr
    #define DBG_WARNING  2, __FILE__, __LINE__, stderr
    #define DBG_INFO     3, __FILE__, __LINE__, stdout

    // debug: check arguments
    #define NULL_PRT_MSG_RET_VAL(ptr, msg, val) \
            if (NULL == (ptr)) { \
                mfs_print(DBG_WARNING, (msg)) ; \
                return (val) ; \
            }
    #define NEG_PRT_MSG_RET_VAL(ret, msg, val) \
            if ((ret) < 0) { \
                mfs_print(DBG_WARNING, (msg)) ; \
                return (val) ; \
            }

    // debug: API
    int mfs_print ( int src_type, char *src_fname, long src_line, FILE *fd, char *msg_fmt, ... ) ;

    // time: API
    long mfs_get_time ( void ) ;

    // thread: API
    int mfs_get_thread_id ( long *th_id ) ;

    // memory: API
    int mfs_malloc  ( char **ptr, long size ) ;
    int mfs_free    ( char **ptr ) ;
    int mfs_realloc ( char **ptr, long new_size ) ;

    // Strings
    int    base_strlen         ( char *str1 ) ;
    int    base_str_equal      ( char *str1, char *str2 ) ;
    char * base_strdup         ( char *str1 ) ;
    int    base_free_and_strdup ( char **ptr, char *str ) ;
    int    base_str_prepare_pathname ( char **buff_data_sys, char *base_dirname, int local_rank, int pathname_length ) ;

#endif

