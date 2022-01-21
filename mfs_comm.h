
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

#ifndef __MFS_COMM_H__
#define __MFS_COMM_H__

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>
  #include <sys/time.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <mpi.h>
  #include "mfs_msg.h"

  int mfs_comm_connect           ( char *srv_name, char *port_name, MPI_Comm *server ) ;

  int mfs_comm_request_send      ( MPI_Comm local,  int rank, int  req_action, int  req_arg1, int  req_arg2 ) ;
  int mfs_comm_request_receive   ( MPI_Comm local,            int *req_action, int *req_arg1, int *req_arg2 ) ;

  int mfs_comm_send_data_to      ( MPI_Comm remote,   int rank, void *buff, int size, int datatype ) ;
  int mfs_comm_recv_data_fromany ( MPI_Comm remote, void *buff,   int size, int datatype ) ;
  int mfs_comm_recv_data_from    ( MPI_Comm remote,   int rank, void *buff, int size, int datatype ) ;

#endif


