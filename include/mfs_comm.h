
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#ifndef __MFS_COMM_COMMON_H__
#define __MFS_COMM_COMMON_H__

    // Includes
    #include "base_lib.h"
    #include "info_params.h"
    #include "info_fsconf.h"


    // File protocol
    #define COMM_NUMBER_OF_PROTOCOLS 3

    #define COMM_USE_LOCAL   1
    #define COMM_USE_MPI     2
    #define COMM_USE_SOCKET  3


    // Datatypes
    typedef struct
    {
	// underlying backend
        int    comm_protocol ;
        char  *comm_protocol_name ;
        int    ns_backend ;

        // accept incomming requests
        char        port_name[MPI_MAX_PORT_NAME] ;
        int         sd ;
	char       *node_str ;
	base_url_t *node_url ;

        // destination
        MPI_Comm  endpoint ;
        int       dd ;

        // MPI identification (MPI_COMM_WORLD)
        int   size ;
        int   rank ;

        // MPI identification (within node)
        MPI_Comm local_comm ;
        int      local_rank ;

        // MPI remote identification
        int   status_rank ;
        int   status_tag ;
        int   status_count ;

        // some stats
        char  is_connected ;
        long  n_send_req ;
        long  n_recv_req ;

    } comm_t ;


    // Macros
    #define mfs_comm_get_rank(pcb)         ((pcb != NULL) ? pcb->rank : -1)
    #define mfs_comm_get_size(pcb)         ((pcb != NULL) ? pcb->size : -1)

    #define mfs_comm_get_status_rank(pcb)  ((pcb != NULL) ? pcb->status_rank : -1)
    #define mfs_comm_get_status_tag(pcb)   ((pcb != NULL) ? pcb->status_tag : -1)
    #define mfs_comm_get_status_count(pcb) ((pcb != NULL) ? pcb->status_count : -1)

    #define mfs_comm_get_nservers(pcb)     (info_fsconf_get_active_nnodes(&((pcb)->partitions)))

    int mfs_comm_reset       ( comm_t *cb ) ;
    int mfs_comm_stats_reset ( comm_t *cb ) ;
    int mfs_comm_stats_show  ( comm_t *cb, char *prefix ) ;

#endif

