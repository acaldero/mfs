
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

#ifndef __MFS_COMM_COMMON_H__
#define __MFS_COMM_COMMON_H__

    // Includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>
    #include <sys/time.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <mpi.h>
    #include "mfs_lib.h"
    #include "mfs_params.h"


    // File protocol
    #define COMM_USE_SOCKET  1
    #define COMM_USE_MPI     2


    // Datatypes
    typedef struct
    {
	// underlying protocol
        int   comm_protocol ;
        char *comm_protocol_name ;

        // server port and name
        char port_name[MPI_MAX_PORT_NAME] ;
        char  srv_name[MPI_MAX_PORT_NAME] ;

        // destination
        MPI_Comm endpoint ;
        int    sd ;
        int   *dd ;

        // local identification
        int   size ;
        int   rank ;

        // remote identification
        int   status_rank ;
        int   status_tag ;
        int   status_count ;

        // some stats
        char  is_connected ;
        long  n_servers ;
        long  n_send_req ;
        long  n_recv_req ;

    } comm_t ;


    // Macros
    #define mfs_comm_get_rank(pcb)         ((pcb != NULL) ? pcb->rank : -1)
    #define mfs_comm_get_size(pcb)         ((pcb != NULL) ? pcb->size : -1)

    #define mfs_comm_get_status_rank(pcb)  ((pcb != NULL) ? pcb->status_rank : -1)
    #define mfs_comm_get_status_tag(pcb)   ((pcb != NULL) ? pcb->status_tag : -1)
    #define mfs_comm_get_status_count(pcb) ((pcb != NULL) ? pcb->status_count : -1)

#endif

