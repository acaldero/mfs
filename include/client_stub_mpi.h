
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

#ifndef __CLIENT_STUB_MPI_H__
#define __CLIENT_STUB_MPI_H__

    // Includes
    #include "base_lib.h"
    #include "info_fsconf.h"
    #include "mfs_comm_common.h"
    #include "mfs_comm_mpi.h"
    #include "stub_msg.h"


    // File System API
    int clientstub_mpi_init     ( comm_t *wb, params_t *params, conf_t *conf ) ;
    int clientstub_mpi_finalize ( comm_t *wb, params_t *params ) ;

    // File API
    long clientstub_mpi_open    ( comm_t *wb, const char *pathname, int flags ) ;
    int  clientstub_mpi_close   ( comm_t *wb, long fd ) ;
    int  clientstub_mpi_read    ( comm_t *wb, long fd, void *buf, int count ) ;
    int  clientstub_mpi_write   ( comm_t *wb, long fd, void *buf, int count ) ;

    // Directory API
    long clientstub_mpi_mkdir   ( comm_t *wb, const char *pathname, int mode ) ;
    long clientstub_mpi_rmdir   ( comm_t *wb, const char *pathname ) ;

    // DBM File API
    long clientstub_mpi_dbmopen   ( comm_t *wb, const char *pathname, int flags ) ;
    int  clientstub_mpi_dbmclose  ( comm_t *wb, long fd ) ;
    int  clientstub_mpi_dbmstore  ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int  count_val ) ;
    int  clientstub_mpi_dbmfetch  ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val ) ;
    int  clientstub_mpi_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key ) ;

#endif

