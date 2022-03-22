
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

#ifndef __MFS_SERVER_STUB_H__
#define __MFS_SERVER_STUB_H__

    // Includes
    #include "mpi.h"
    #include "mfs_lib.h"
    #include "mfs_protocol.h"
    #include "mfs_files.h"
    #include "mfs_directories.h"
    #include "mfs_dbm.h"


    // Define
    #define SRVSTUB_READ_UP_TO_IS_DYNAMIC (1*1024*1024)
    #define SRVSTUB_READ_BUFFLOCAL_SIZE       (64*1024)
    #define SRVSTUB_WRITE_MAXBUFF_SIZE   (10*1024*1024)



    // Datatypes
    typedef struct
    {
        // underlying comm and file
        comm_t  wb ;
        file_t  fd ;

        // underlying file options
        char   *data_prefix ;
        int     file_backend ;

    } srvstub_t ;


    // General API
    int serverstub_init           ( comm_t *wb, params_t *params ) ;
    int serverstub_finalize       ( comm_t *wb, params_t *params ) ;
    int serverstub_accept         ( comm_t *ab, params_t *params, comm_t *wb ) ;
    int serverstub_disconnect_all ( comm_t *ab, params_t *params ) ;

    //  File API
    int serverstub_open       ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags ) ;
    int serverstub_close      ( comm_t *ab, params_t *params, int  fd ) ;
    int serverstub_read       ( comm_t *ab, params_t *params, int  fd, int count ) ;
    int serverstub_write      ( comm_t *ab, params_t *params, int  fd, int count ) ;

    //  Directory API
    int serverstub_mkdir      ( comm_t *ab, params_t *params, int pathname_length, int mode ) ;
    int serverstub_rmdir      ( comm_t *ab, params_t *params, int pathname_length ) ;

    //  DBM File API
    int serverstub_dbmopen    ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags ) ;
    int serverstub_dbmclose   ( comm_t *ab, params_t *params, int  fd ) ;
    int serverstub_dbmstore   ( comm_t *ab, params_t *params, int  fd, int count ) ;
    int serverstub_dbmfetch   ( comm_t *ab, params_t *params, int  fd, int count ) ;
    int serverstub_dbmdelete  ( comm_t *ab, params_t *params, int  fd, int count ) ;

#endif

