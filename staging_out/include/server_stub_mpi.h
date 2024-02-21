
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

#ifndef __SERVER_STUB_MPI_H__
#define __SERVER_STUB_MPI_H__

    // Includes
    #include "mpi.h"
    #include "base_lib.h"
    #include "mfs_files.h"
    #include "mfs_directories.h"
    #include "mfs_dbm.h"
    #include "mfs_comm_mpi.h"
    #include "stub_msg.h"


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
        int     file_backend ;

    } srvstub_t ;


    // General API
    int serverstub_mpi_init           ( comm_t *wb, params_t *params ) ;
    int serverstub_mpi_finalize       ( comm_t *wb, params_t *params ) ;
    int serverstub_mpi_accept         ( comm_t *ab, params_t *params, comm_t *wb ) ;
    int serverstub_mpi_disconnect_all ( comm_t *ab, params_t *params ) ;

    //  File API
    int serverstub_mpi_open       ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags ) ;
    int serverstub_mpi_close      ( comm_t *ab, params_t *params, int  fd ) ;
    int serverstub_mpi_read       ( comm_t *ab, params_t *params, int  fd, int count ) ;
    int serverstub_mpi_write      ( comm_t *ab, params_t *params, int  fd, int count ) ;

    //  Directory API
    int serverstub_mpi_mkdir      ( comm_t *ab, params_t *params, int pathname_length, int mode ) ;
    int serverstub_mpi_rmdir      ( comm_t *ab, params_t *params, int pathname_length ) ;

    //  DBM File API
    int serverstub_mpi_dbmopen    ( comm_t *ab, params_t *params, int *fd, int pathname_length, int flags ) ;
    int serverstub_mpi_dbmclose   ( comm_t *ab, params_t *params, int  fd ) ;
    int serverstub_mpi_dbmstore   ( comm_t *ab, params_t *params, int  fd, int key_size, int val_size ) ;
    int serverstub_mpi_dbmfetch   ( comm_t *ab, params_t *params, int  fd, int key_size, int val_size ) ;
    int serverstub_mpi_dbmdelete  ( comm_t *ab, params_t *params, int  fd, int key_size ) ;

#endif

