
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

#ifndef __MFS_COMM_H__
#define __MFS_COMM_H__

    // Includes
    #include "base_lib.h"
    #include "info_params.h"

    // File protocol
    #define COMM_NUMBER_OF_PROTOCOLS 3

    #define COMM_USE_LOCAL   1
    #define COMM_USE_MPI     2
    #define COMM_USE_SOCKET  3


    // Datatypes
    class mfs_comm
    {
      protected:
        // underlying backend
        int   comm_backend ;
        char *comm_backend_name ;

        // some stats
        char  is_connected ;
        long  n_send_req ;
        long  n_recv_req ;

      public:
        int reset       ( void ) ;
        int stats_reset ( void ) ;
        int stats_show  ( char *prefix ) ;
    } ;

#endif

