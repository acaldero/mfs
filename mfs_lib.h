
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
  #include <stdarg.h>
  #include <fcntl.h>
  #include <thread>
  #include <chrono>
  #include <mpi.h>
  #include "mfs_msg.h"


  // print for debug
  #define DBG_ERROR    1, __FILE__, __LINE__, stderr
  #define DBG_WARNING  2, __FILE__, __LINE__, stderr
  #define DBG_INFO     3, __FILE__, __LINE__, stdout

  int mfs_print ( int src_type, char *src_fname, long src_line, FILE *fd, char *msg_fmt, ... ) ;


  // server_port
  int mfs_write_server_port ( char *port_name, int rank ) ;
  int mfs_read_server_port  ( char *port_name, int rank ) ;

#endif

