
/*
 *  Copyright 2020-2021 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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
    #include "mfs_files.h"
    
    // Const
    #define MFS_DATA_PREFIX "./data/"
    
    // Datatypes
    typedef struct
    {
        // server identification
        int  size ;
        int  rank ;
        char port_name[MPI_MAX_PORT_NAME] ;

        // associated client
        MPI_Comm client ;
        int      tag_id ;

    } server_stub_t ;
    
    
    // General API
    int serverstub_init         ( server_stub_t *wb, int *argc, char ***argv ) ;
    int serverstub_finalize     ( server_stub_t *wb ) ;
    int serverstub_accept       ( server_stub_t *ab, server_stub_t *wb ) ;
    int serverstub_disconnect   ( server_stub_t *ab ) ;
    int serverstub_request_recv ( server_stub_t *ab, void *buff, int size, int datatype ) ;
    int serverstub_request_send ( server_stub_t *ab, void *buff, int size, int datatype ) ;

    // File System API
    int serverstub_open  ( server_stub_t *ab, int pathname_length, int flags ) ;
    int serverstub_close ( server_stub_t *ab, int fd ) ;
    int serverstub_read  ( server_stub_t *ab, int fd, int count ) ;
    int serverstub_write ( server_stub_t *ab, int fd, int count ) ;

#endif

