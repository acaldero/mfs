
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


#include "mfs_api.h"


/*
 *  File System API
 */

int mfs_api_init ( comm_t *wb, params_t *params, int *argc, char ***argv )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,     "[MFS_API]: NULL wb     :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[MFS_API]: NULL params :-(\n", -1) ;

    // Get arguments..
    ret = info_params_get(params, argc, argv) ;
    if (ret < 0) {
        info_params_show_usage() ;
	return -1 ;
    }
    if (params->verbose > 0) {
        info_params_show(params) ;
    }

    // Open server port...
    wb->comm_protocol = params->comm_backend ;
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_init(wb, params) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_init(wb, params) ;
             break ;

        case COMM_USE_LOCAL:
             ret = clientstub_local_init(wb, params) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_api_finalize ( comm_t *wb, params_t *params )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,     "[MFS_API]: NULL wb     :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[MFS_API]: NULL params :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_finalize(wb, params) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_finalize(wb, params) ;
             break ;

        case COMM_USE_LOCAL:
             ret = clientstub_local_finalize(wb, params) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Finalize params
    if (ret >= 0)
    {
        info_params_free(params) ;
    }

    // Return OK
    return ret ;
}

int mfs_api_open_partition ( comm_t *wb, params_t *params, char *conf_fname )
{
    int ret ;
    int n_servers ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,     "[MFS_API]: NULL wb     :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[MFS_API]: NULL params :-(\n", -1) ;

    // Get partitions..
    ret = info_fsconf_get(&(wb->partitions), conf_fname) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[MFS_API]: info_fsconf_get fails to read file '%s' :-(\n", conf_fname) ;
        return -1 ;
    }
    if (params->verbose > 0) {
        info_fsconf_show(&(wb->partitions)) ;
    }

    // Check partition...
    if (wb->partitions.n_partitions < 1) {
        mfs_print(DBG_ERROR, "[MFS_API]: info_fsconf_get fails to read at least one partition in file '%s' :-(\n", conf_fname) ;
        return -1 ;
    }
    n_servers = info_fsconf_get_partition_nnodes(wb->partitions.active) ;
    if (n_servers < 0) {
        mfs_print(DBG_ERROR, "[MFS_API]: negative number of servers in partition :-(\n") ;
        return -1 ;
    }

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
             ret = clientstub_socket_open_partition(wb, params, wb->partitions.active) ;
             break ;

        case COMM_USE_MPI:
             ret = clientstub_mpi_open_partition(wb, params, wb->partitions.active) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_open_partition(wb, params, wb->partitions.active) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_api_close_partition ( comm_t *wb, params_t *params )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,     "[MFS_API]: NULL wb     :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[MFS_API]: NULL params :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
             ret = clientstub_socket_close_partition(wb, params, wb->partitions.active) ;
             break ;

        case COMM_USE_MPI:
             ret = clientstub_mpi_close_partition(wb, params, wb->partitions.active) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_close_partition(wb, params, wb->partitions.active) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Finalize configuration
    if (ret >= 0)
    {
        info_fsconf_free(&(wb->partitions)) ;
    }

    // Return OK
    return ret ;
}


/*
 *  File API
 */

long mfs_api_open ( comm_t *wb, const char *pathname, int flags )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_open(wb, pathname, flags) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_open(wb, pathname, flags) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_open(wb, pathname, flags) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_close ( comm_t *wb, long fd )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_close(wb, fd) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_close(wb, fd) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_close(wb, fd) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_api_read ( comm_t *wb, long fd, void *buff_char, int count )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_read(wb, fd, buff_char, count) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_read(wb, fd, buff_char, count) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_read(wb, fd, buff_char, count) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_api_write ( comm_t *wb, long fd, void *buff_char, int count )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_write(wb, fd, buff_char, count) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_write(wb, fd, buff_char, count) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_write(wb, fd, buff_char, count) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}


/*
 *  Directory API
 */

long mfs_api_mkdir ( comm_t *wb, const char *pathname, int mode )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_mkdir(wb, pathname, mode) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_mkdir(wb, pathname, mode) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_mkdir(wb, pathname, mode) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

long mfs_api_rmdir ( comm_t *wb, const char *pathname )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_rmdir(wb, pathname) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_rmdir(wb, pathname) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_rmdir(wb, pathname) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}


/*
 *  DBM File API
 */

long mfs_api_dbmopen ( comm_t *wb, const char *pathname, int flags )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmopen(wb, pathname, flags) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmopen(wb, pathname, flags) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmopen(wb, pathname, flags) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmclose ( comm_t *wb, long fd )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmclose(wb, fd) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmclose(wb, fd) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmclose(wb, fd) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmstore ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmstore(wb, fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmstore(wb, fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmstore(wb, fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmfetch ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmfetch(wb, fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmfetch(wb, fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmfetch(wb, fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmdelete(wb, fd, buff_key, count_key) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmdelete(wb, fd, buff_key, count_key) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmdelete(wb, fd, buff_key, count_key) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

