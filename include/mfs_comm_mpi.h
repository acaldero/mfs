
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

#ifndef __MFS_COMM_MPI_H__
#define __MFS_COMM_MPI_H__

    // Includes
    #include "base_lib.h"
    #include "info_params.h"
    #include "info_fsconf.h"
    #include "mfs_comm.h"
    #include "stub_msg.h"

    #include <mpi.h>


    // API
    int mfs_comm_mpi_init           ( comm_t *cb, conf_part_t *partition, int *main_argc, char ***main_argv ) ;

    int mfs_comm_mpi_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype ) ;
    int mfs_comm_mpi_send_data_to   ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype ) ;

    int mfs_comm_mpi_send_request          ( comm_t *wb, int   rank,      long action, long arg1, long arg2, long arg3 ) ;
    int mfs_comm_mpi_receive_request       ( comm_t *wb, int   rank,      msg_t *msg ) ;

    int mfs_comm_mpi_send_buffer_in_chunks ( comm_t *wb, void *buff_char, int  count,  int  buffer_size ) ;

    int mfs_comm_mpi_stats_reset    ( comm_t *cb ) ;
    int mfs_comm_mpi_stats_show     ( comm_t *cb, char *prefix ) ;

#endif

