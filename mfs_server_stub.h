
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
    
    
    // Datatypes
    typedef struct
    {
        // server identification
        int  size ;
        int  rank ;
        char port_name[MPI_MAX_PORT_NAME] ;

        // server activities
        int  the_end ;

        // associated client
        MPI_Comm client ;
        int      client_rank ;

    } server_stub_t ;
    
    
    // API
    int serverstub_init         ( server_stub_t *wb, int *argc, char ***argv ) ;
    int serverstub_finalize     ( server_stub_t *wb ) ;

    int serverstub_get_theend   ( server_stub_t *wb ) ;
    int serverstub_set_theend   ( server_stub_t *wb, int value ) ;

    int serverstub_accept       ( server_stub_t *ab, server_stub_t *wb ) ;
    int serverstub_request_recv ( server_stub_t *ab, void *buff_int, int size ) ;
    int serverstub_request_send ( server_stub_t *ab, void *buff_int, int size ) ;

#endif

