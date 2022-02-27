
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

#ifndef __MFS_COMM_MPI_H__
#define __MFS_COMM_MPI_H__

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
    #include "mfs_comm_common.h"


    // API
    int mfs_comm_mpi_init     ( comm_t *cb, params_t *params ) ;
    int mfs_comm_mpi_finalize ( comm_t *cb ) ;

    int mfs_comm_mpi_register   ( comm_t *cb ) ;
    int mfs_comm_mpi_unregister ( comm_t *cb ) ;
    int mfs_comm_mpi_accept     ( comm_t *ab ) ;
    int mfs_comm_mpi_connect    ( comm_t *cb ) ;
    int mfs_comm_mpi_disconnect ( comm_t *cb ) ;

    int mfs_comm_mpi_recv_data_from ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype ) ;
    int mfs_comm_mpi_send_data_to   ( comm_t *cb, int rank, void *buff, int size, MPI_Datatype datatype ) ;

#endif

