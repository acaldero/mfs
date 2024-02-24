
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

#ifndef __MFS_COMM_MPI_H__
#define __MFS_COMM_MPI_H__

    // Includes
    #include "base_lib.h"
    #include "info_params.h"
    #include "mfs_comm.h"

    #include <mpi.h>


    // Datatypes
    class mfs_comm_mpi : mfs_comm
    {
      protected:
        // destination
        MPI_Comm  endpoint ;

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

        // accept incomming requests
        char        port_name[MPI_MAX_PORT_NAME] ;
        int         sd ;
	char       *node_str ;
//	base_url_t *node_url ;
	char       *node_url ;

      public:
   //   int recv_data_from        ( int rank, void *buff, int size ) ;
        int recv_data_from        ( int rank, void *buff, int size, MPI_Datatype datatype ) ;
   //   int send_data_to          ( int rank, void *buff, int size ) ;
        int send_data_to          ( int rank, void *buff, int size, MPI_Datatype datatype ) ;

        int send_request          ( int rank, long  action, long  arg1, long  arg2, long  arg3 ) ;
        int receive_request       ( int rank, long *action, long *arg1, long *arg2, long *arg3 ) ;

        int send_buffer_in_chunks ( void *buff_char, int  count,  int  buffer_size ) ;

	// getters
        int mfs_comm_get_rank() { return this->rank ; }
        int mfs_comm_get_size() { return this->size ; }

        int mfs_comm_get_status_rank()  { return this->status_rank ; }
        int mfs_comm_get_status_tag()   { return this->status_tag ;  }
        int mfs_comm_get_status_count() { return this->status_count ; }
    } ;

#endif

