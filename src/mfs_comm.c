
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

#include "mfs_comm.h"


int mfs_comm::reset ( void )
{
    this->comm_backend = 0 ;
    this->comm_backend_name = NULL ;

    this->is_connected = 0 ;
    this->n_send_req = 0L ;
    this->n_recv_req = 0L ;

    // Return OK
    return 0 ;
}


//
// Stats
//

int mfs_comm::stats_reset ( void )
{
    // cb->... (stats)
    this->n_send_req = 0 ;
    this->n_recv_req = 0 ;

    // Return OK
    return 0 ;
}

int mfs_comm::stats_show  ( char *prefix )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(prefix, "[COMM] NULL prefix", -1) ;

    // Print stats...
    printf("%s: Comm:\n",              prefix) ;
    printf("%s: + # send=%d\n",        prefix, this->n_send_req) ;
    printf("%s: + # recv=%d\n",        prefix, this->n_recv_req) ;

    // Return OK
    return 1 ;
}


