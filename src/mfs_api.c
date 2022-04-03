
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


#include "mfs_api.h"


/*
 *  File System API
 */

int mfs_api_init ( mfs_t *wb, params_t *params, int *argc, char ***argv )
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

    // Initialize values...
    memset(wb, 0, sizeof(mfs_t)) ;

    // Initialize cb elements...
    ret = clientstub_mpi_init   (&(wb->cb[0]), params) ;
    ret = clientstub_socket_init(&(wb->cb[1]), params) ;
    ret = clientstub_local_init (&(wb->cb[2]), params) ;

    // Return OK
    return ret ;
}

int mfs_api_finalize ( mfs_t *wb, params_t *params )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,     "[MFS_API]: NULL wb     :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[MFS_API]: NULL params :-(\n", -1) ;

    // Finalize cb elements...
    ret = clientstub_mpi_finalize   (&(wb->cb[0]), params) ;
    ret = clientstub_socket_finalize(&(wb->cb[1]), params) ;
    ret = clientstub_local_finalize (&(wb->cb[2]), params) ;

    // Finalize params
    if (ret >= 0)
    {
        info_params_free(params) ;
    }

    // Return OK
    return ret ;
}

int mfs_api_open_partition ( mfs_t *wb, params_t *params, char *conf_fname )
{
    int         ret ;
    base_url_t *elto_url ; 

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

    // Initialize wb with partition related info...
    wb->n_eltos = info_fsconf_get_partition_nnodes(wb->partitions.active) ;
    if (wb->n_eltos < 0) {
        mfs_print(DBG_ERROR, "[MFS_API]: negative number of servers in partition :-(\n") ;
        return -1 ;
    }

    wb->wb = (comm_t *)malloc(wb->n_eltos * sizeof(comm_t)) ;
    if (NULL == wb->wb) {
        mfs_print(DBG_ERROR, "[MFS_API]: malloc return NULL :-(\n") ;
        return -1 ;
    }

    // Initialize elements...
    for (int i=0; i<wb->n_eltos; i++)
    {
        elto_url = info_fsconf_get_active_url(&(wb->partitions), i) ;
        if (!strcmp("mpiServer", elto_url->protocol))
        {
	     memcpy(&(wb->wb[i]), (void *)&(wb->cb[0]), sizeof(comm_t)) ;
             ret = clientstub_mpi_open_partition_element(&(wb->wb[i]), params, wb->partitions.active, i) ;
        }
        if (!strcmp("tcpServer", elto_url->protocol))
        {
	     memcpy(&(wb->wb[i]), (void *)&(wb->cb[1]), sizeof(comm_t)) ;
             ret = clientstub_socket_open_partition_element(&(wb->wb[i]), params, wb->partitions.active, i) ;
        }
        if (!strcmp("file",      elto_url->protocol))
        {
	     memcpy(&(wb->wb[i]), (void *)&(wb->cb[2]), sizeof(comm_t)) ;
	     ret = clientstub_local_open_partition_element(&(wb->wb[i]), params, wb->partitions.active, i) ;
        }
    }

    // Return OK
    return ret ;
}

int mfs_api_close_partition ( mfs_t *wb, params_t *params )
{
    int ret ;
    base_url_t *elto_url ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,     "[MFS_API]: NULL wb     :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(params, "[MFS_API]: NULL params :-(\n", -1) ;

    // Close elements...
    for (int i=0; i<wb->n_eltos; i++)
    {
        elto_url = info_fsconf_get_active_url(&(wb->partitions), i) ;
        if (!strcmp("mpiServer", elto_url->protocol))
        {
             ret = clientstub_mpi_close_partition_element(&(wb->wb[i]), params, wb->partitions.active) ;
        }
        if (!strcmp("tcpServer", elto_url->protocol))
        {
             ret = clientstub_socket_close_partition_element(&(wb->wb[i]), params, wb->partitions.active) ;
        }
        if (!strcmp("file",      elto_url->protocol))
        {
	     ret = clientstub_local_close_partition_element(&(wb->wb[i]), params, wb->partitions.active) ;
        }
    }

    // Finalize vector
    wb->n_eltos = 0 ;
    free(wb->wb) ;

    // Finalize configuration
    if (ret >= 0) {
        info_fsconf_free(&(wb->partitions)) ;
    }

    // Return OK
    return ret ;
}


/*
 *  File API
 */

long mfs_api_open ( mfs_t *wb, const char *pathname, int flags )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_open(&(wb->wb[0]), pathname, flags) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_open(&(wb->wb[0]), pathname, flags) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_open(&(wb->wb[0]), pathname, flags) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_close ( mfs_t *wb, long fd )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_close(&(wb->wb[0]), fd) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_close(&(wb->wb[0]), fd) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_close(&(wb->wb[0]), fd) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_api_read ( mfs_t *wb, long fd, void *buff_char, int count )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_read(&(wb->wb[0]), fd, buff_char, count) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_read(&(wb->wb[0]), fd, buff_char, count) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_read(&(wb->wb[0]), fd, buff_char, count) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int mfs_api_write ( mfs_t *wb, long fd, void *buff_char, int count )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_write(&(wb->wb[0]), fd, buff_char, count) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_write(&(wb->wb[0]), fd, buff_char, count) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_write(&(wb->wb[0]), fd, buff_char, count) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}


/*
 *  Directory API
 */

long mfs_api_mkdir ( mfs_t *wb, const char *pathname, int mode )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_mkdir(&(wb->wb[0]), pathname, mode) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_mkdir(&(wb->wb[0]), pathname, mode) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_mkdir(&(wb->wb[0]), pathname, mode) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

long mfs_api_rmdir ( mfs_t *wb, const char *pathname )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_rmdir(&(wb->wb[0]), pathname) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_rmdir(&(wb->wb[0]), pathname) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_rmdir(&(wb->wb[0]), pathname) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}


/*
 *  DBM File API
 */

long mfs_api_dbmopen ( mfs_t *wb, const char *pathname, int flags )
{
    int ret ;
    int fd ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmopen(&(wb->wb[0]), pathname, flags) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmopen(&(wb->wb[0]), pathname, flags) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmopen(&(wb->wb[0]), pathname, flags) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmclose ( mfs_t *wb, long fd )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmclose(&(wb->wb[0]), fd) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmclose(&(wb->wb[0]), fd) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmclose(&(wb->wb[0]), fd) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmstore ( mfs_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmstore(&(wb->wb[0]), fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmstore(&(wb->wb[0]), fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmstore(&(wb->wb[0]), fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmfetch ( mfs_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmfetch(&(wb->wb[0]), fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmfetch(&(wb->wb[0]), fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmfetch(&(wb->wb[0]), fd, buff_key, count_key, buff_val, count_val) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_dbmdelete ( mfs_t *wb, long fd, void *buff_key, int count_key )
{
    int ret ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(wb, "[MFS_API]: NULL wb :-(\n", -1) ;

    // Open server port...
    switch (wb->wb[0].comm_protocol)
    {
        case COMM_USE_SOCKET:
	     ret = clientstub_socket_dbmdelete(&(wb->wb[0]), fd, buff_key, count_key) ;
             break ;

        case COMM_USE_MPI:
	     ret = clientstub_mpi_dbmdelete(&(wb->wb[0]), fd, buff_key, count_key) ;
             break ;

        case COMM_USE_LOCAL:
	     ret = clientstub_local_dbmdelete(&(wb->wb[0]), fd, buff_key, count_key) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[MFS_API]: ERROR on comm_protocol(%d).\n", wb->wb[0].comm_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int  mfs_api_get_rank  ( mfs_t *wb )
{
    // Check params...
    NULL_PRT_MSG_RET_VAL(wb,     "[MFS_API]: NULL wb     :-(\n", -1) ;
    NULL_PRT_MSG_RET_VAL(wb->wb, "[MFS_API]: NULL wb->wb :-(\n", -1) ;

    return wb->wb[0].rank ;
}

