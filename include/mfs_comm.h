
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

#ifndef __MFS_COMM_H__
#define __MFS_COMM_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_params.h"
    #include "mfs_comm_mpi.h"
    #include "mfs_comm_socket.h"
    #include "mfs_params.h"


    // Communications
    int mfs_comm_init       ( comm_t *cb, conf_part_t *partition, params_t *params ) ;
    int mfs_comm_finalize   ( comm_t *cb ) ;

    int mfs_comm_register   ( comm_t *cb ) ;
    int mfs_comm_unregister ( comm_t *cb ) ;

    int mfs_comm_accept           ( comm_t *ab, comm_t *wb ) ;
    int mfs_comm_interconnect_all ( comm_t *cb, conf_t *conf ) ;
    int mfs_comm_disconnect_all   ( comm_t *cb ) ;

    int mfs_comm_request_send       ( comm_t *cb, int rank, long  req_action, long  req_arg1, long  req_arg2 ) ;
    int mfs_comm_request_receive    ( comm_t *cb,           long *req_action, long *req_arg1, long *req_arg2 ) ;

    int mfs_comm_recv_data_from     ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype ) ;
    int mfs_comm_send_data_to       ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype ) ;

    int mfs_comm_stats_reset        ( comm_t *cb ) ;
    int mfs_comm_stats_show         ( comm_t *cb, char *prefix ) ;

#endif

