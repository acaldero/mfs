
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

#ifndef __MFS_COMM_SOCKET_H__
#define __MFS_COMM_SOCKET_H__

    // Includes
    #include "mfs_lib.h"
    #include "mfs_comm_common.h"
    #include "mfs_ns.h"

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/tcp.h>


    // Consts
    #define ONE_MB     (1024 * 1024)


    // API
    int mfs_comm_socket_init       ( comm_t *cb, conf_part_t *partition, int server_port, int ns_backend ) ;
    int mfs_comm_socket_finalize   ( comm_t *cb ) ;

    int mfs_comm_socket_accept     ( comm_t *ab, int remote_rank ) ;
    int mfs_comm_socket_connect    ( comm_t *cb, char *srv_uri, int remote_rank ) ;
    int mfs_comm_socket_close      ( int *sd ) ;

    int mfs_comm_socket_recv_data_from ( comm_t *cb, int rank, void *buff, int size ) ;
    int mfs_comm_socket_send_data_to   ( comm_t *cb, int rank, void *buff, int size ) ;

    int mfs_comm_socket_stats_reset    ( comm_t *cb ) ;
    int mfs_comm_socket_stats_show     ( comm_t *cb, char *prefix ) ;

#endif

