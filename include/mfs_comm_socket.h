
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#ifndef __MFS_COMM_SOCKET_H__
#define __MFS_COMM_SOCKET_H__

    // Includes
    #include "base_lib.h"
    #include "info_params.h"
    #include "mfs_comm.h"

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/tcp.h>


    // Datatypes
    class mfs_comm_socket : mfs_comm
    {
      protected:
        // destination
        int  dd ;

      public:
        int recv_data_from  ( int rank, void *buff, int size ) ;
        int send_data_to    ( int rank, void *buff, int size ) ;

        int send_request    ( int rank, long  action, long  arg1, long  arg2, long  arg3 ) ;
        int receive_request ( int rank, long *action, long *arg1, long *arg2, long *arg3 ) ;
    } ;

#endif

