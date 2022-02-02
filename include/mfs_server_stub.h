
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

#ifndef __MFS_SERVER_STUB_H__
#define __MFS_SERVER_STUB_H__

    // Includes
    #include "mpi.h"
    #include "mfs_lib.h"
    #include "mfs_protocol.h"
    #include "mfs_files.h"
    #include "mfs_directories.h"


    // Datatypes
    typedef struct
    {
        // underlying comm and file
        comm_t  wb ;
        file_t  fd ;

        // underlying file options
        char   *data_prefix ;
        int     file_protocol ;

    } srvstub_t ;


    // General API
    int  serverstub_init         ( comm_t *wb, params_t *params ) ;
    int  serverstub_finalize     ( comm_t *wb ) ;
    int  serverstub_accept       ( comm_t *ab, comm_t *wb ) ;
    int  serverstub_disconnect   ( comm_t *ab ) ;

    // File System API
    int  serverstub_open    ( comm_t *ab, file_t *fd, char *base_dirname, int pathname_length, int flags ) ;
    int  serverstub_close   ( comm_t *ab, file_t *fd ) ;
    int  serverstub_read    ( comm_t *ab, file_t *fd, int count ) ;
    int  serverstub_write   ( comm_t *ab, file_t *fd, int count ) ;

    // Directory API
    int  serverstub_mkdir   ( comm_t *ab, char *base_dirname, int pathname_length, int mode ) ;
    int  serverstub_rmdir   ( comm_t *ab, char *base_dirname, int pathname_length ) ;

#endif
