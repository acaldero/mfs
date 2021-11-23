
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

#ifndef __MFS_LIB_H__
#define __MFS_LIB_H__

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <thread>
  #include <condition_variable>
  #include <chrono>
  #include <mfs_lib.h>
  #include <mpi.h>

  // server_port
  int mfs_write_server_port ( char *port_name ) ;
  int mfs_read_server_port  ( char *port_name ) ;

  // messages
  #define REQ_ACTION_END        0
  #define REQ_ACTION_DISCONNECT 1
  #define REQ_ACTION_DATA       2

  int send_request ( MPI_Comm server, int  req_action, int  req_id ) ;
  int recv_request ( MPI_Comm client, int *req_action, int *req_id ) ;

#endif

