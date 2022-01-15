
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

#ifndef __MFS_CLIENT_STUB_H__
#define __MFS_CLIENT_STUB_H__

    // Includes
    #include "mpi.h"
    #include "mfs_lib.h"
    
    
    // Datatypes
    typedef struct
    {
        // server port and comm
        char port_name[MPI_MAX_PORT_NAME] ;
        char  srv_name[MPI_MAX_PORT_NAME] ;
        MPI_Comm server ;
    
        // client identification
        int  size ;
        int  rank ;
    
    } client_stub_t ;
    
    
    // File System API
    int clientstub_init     ( client_stub_t *wb, int *argc, char ***argv ) ;
    int clientstub_finalize ( client_stub_t *wb ) ;

    int clientstub_open  ( client_stub_t *wb, const char *pathname, int flags ) ;
    int clientstub_close ( client_stub_t *wb, int fd ) ;
    int clientstub_read  ( client_stub_t *wb, int fd, void *buf, int count ) ;
    int clientstub_write ( client_stub_t *wb, int fd, void *buf, int count ) ;

#endif

