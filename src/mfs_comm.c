
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

#include "mfs_comm.h"


int mfs_comm_reset ( comm_t *cb )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(cb, "[COMM]: NULL cb :-(\n", -1) ;

    // Fill default values
    cb->comm_protocol      = -1 ;
    cb->comm_protocol_name = NULL ;

    cb->n_send_req    =  0 ;
    cb->n_recv_req    =  0 ;

    cb->status_rank   = -1 ;
    cb->status_tag    = -1 ;
    cb->status_count  = -1 ;

    cb->rank          = 0 ;
    cb->size          = 0 ;
    cb->ns_backend    = 0 ;
    cb->is_connected  = 0 ;

    // Return OK
    return 0 ;
}


//
// Stats
//

int mfs_comm_stats_reset ( comm_t *cb )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(cb, "[COMM]: NULL cb :-(\n", -1) ;

    // cb->... (stats)
    cb->n_send_req = 0 ;
    cb->n_recv_req = 0 ;

    // Return OK
    return 0 ;
}

int mfs_comm_stats_show  ( comm_t *cb, char *prefix )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(cb, "[COMM] NULL cb", -1) ;

    // Print stats...
    printf("%s: Comm:\n",              prefix) ;
    printf("%s: + # servers=%d\n",     prefix, cb->n_servers) ;
    printf("%s: + # send=%d\n",        prefix, cb->n_send_req) ;
    printf("%s: + # recv=%d\n",        prefix, cb->n_recv_req) ;

    // Return OK
    return 1 ;
}

