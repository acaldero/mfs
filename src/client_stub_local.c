
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


#include "client_stub_local.h"


/*
 *  File System API
 */

int clientstub_local_init ( comm_t *wb, params_t *params )
{
    int ret = 0 ;

    // Initialization
    mfs_comm_reset(wb) ;
    wb->comm_protocol = COMM_USE_LOCAL ;
    wb->comm_protocol_name = "LOCAL" ;

    // Initialize files, dbm and directories
    ret = mfs_file_init() ;
    if (ret < 0) { return -1 ; }

    ret = mfs_dbm_init() ;
    if (ret < 0) { return -1 ; }

    ret = mfs_directory_init() ;
    if (ret < 0) { return -1 ; }

    // Return OK/KO
    return ret ;
}

int clientstub_local_finalize ( comm_t *wb, params_t *params )
{
    int ret = 0 ;

    // Finalize files, dbm and directories
    ret = mfs_directory_finalize() ;
    if (ret < 0) { return -1 ; }

    ret = mfs_file_finalize() ;
    if (ret < 0) { return -1 ; }

    ret = mfs_dbm_finalize() ;
    if (ret < 0) { return -1 ; }

    // Return OK/KO
    return ret ;
}

int clientstub_local_open_partition_element ( comm_t *wb, params_t *params, conf_part_t *partition, int remote_rank )
{
    int  local_rank ;

    // Get working node
    local_rank = mfs_comm_get_rank(wb) ;

    wb->node_str = info_fsconf_get_partition_node(partition, local_rank) ;
    if (NULL == wb->node_str) {
        mfs_print(DBG_ERROR, "Client[%d]: info_fsconf_get_active_node return NULL :-(\n", local_rank) ;
	return -1 ;
    }

    wb->node_url = info_fsconf_get_partition_url(partition, local_rank) ;
    if (NULL == wb->node_url) {
        mfs_print(DBG_ERROR, "Client[%d]: info_fsconf_get_active_url return NULL :-(\n", local_rank) ;
	return -1 ;
    }

    wb->is_connected = 1 ;

    // Return OK
    return 1 ;
}

int clientstub_local_close_partition_element ( comm_t *wb, params_t *params, conf_part_t *partition )
{
    wb->is_connected = 0 ;

    // Return OK
    return 1 ;
}


/*
 *  File API
 */

long clientstub_local_open ( comm_t *wb, const char *pathname, int flags )
{
     int    ret, fd ;
     char  *buff_data_sys ;
     int    local_rank ;

     mfs_print(DBG_INFO, "Client[%d]: open('%s', %d)\n", mfs_comm_get_rank(wb), pathname, flags) ;

     // Initialize...
     ret = 0 ;
     buff_data_sys = NULL ;

     // Get working node
     local_rank = mfs_comm_get_rank(wb) ;

     // Get filename
     ret = base_str_prepare_pathname(&buff_data_sys, wb->node_url->file, local_rank, strlen(pathname)) ;
     if (ret < 0) {
         mfs_print(DBG_ERROR, "Client[%d]: base_str_prepare_pathname(%d) fails :-(\n", mfs_comm_get_rank(wb), strlen(pathname)) ;
	 return -1 ;
     }
     strcat(buff_data_sys, pathname) ;

     // Open file
     mfs_print(DBG_INFO, "Client[%d]: open('%s', %d)\n", mfs_comm_get_rank(wb), buff_data_sys, flags) ;

     ret = mfs_file_open(&fd, FILE_USE_POSIX, buff_data_sys, flags) ;
     if (ret < 0) {
	 mfs_print(DBG_WARNING, "Client[%d]: file '%s' not opened :-(\n", mfs_comm_get_rank(wb), pathname) ;
	 return -1 ;
     }

     // free filename buffer
     ret = mfs_free(&buff_data_sys) ;
     if (ret < 0) {
         mfs_print(DBG_WARNING, "Client[%d]: problem on free :-(\n", mfs_comm_get_rank(wb)) ;
     }

     return fd ;
}

int  clientstub_local_close ( comm_t *wb, long fd )
{
     mfs_print(DBG_INFO, "Client[%d]: close(%d)\n", mfs_comm_get_rank(wb), fd) ;

     return mfs_file_close(fd) ;
}

int  clientstub_local_read ( comm_t *wb, long fd, void *buff_char, int count )
{
     mfs_print(DBG_INFO, "Client[%d]: read(%d, %p, %d)\n", mfs_comm_get_rank(wb), fd, buff_char, count) ;

     return mfs_file_read(fd, buff_char, count) ;
}

int  clientstub_local_write ( comm_t *wb, long fd, void *buff_char, int count )
{
     mfs_print(DBG_INFO, "Client[%d]: write(%d, %p, %d)\n", mfs_comm_get_rank(wb), fd, buff_char, count) ;

     return mfs_file_write(fd, buff_char, count) ;
}


/*
 *  Directory API
 */

long clientstub_local_mkdir ( comm_t *wb, const char *pathname, int mode )
{
     mfs_print(DBG_INFO, "Client[%d]: mkdir('%s', %d)\n", mfs_comm_get_rank(wb), pathname, mode) ;

     return mfs_directory_mkdir(DIRECTORY_USE_POSIX, (char *)pathname, mode) ;
}

long clientstub_local_rmdir ( comm_t *wb, const char *pathname )
{
     mfs_print(DBG_INFO, "Client[%d]: rmdir('%s')\n", mfs_comm_get_rank(wb), pathname) ;

     return mfs_directory_rmdir(DIRECTORY_USE_POSIX, (char *)pathname) ;
}


/*
 *  DBM File API
 */

long clientstub_local_dbmopen ( comm_t *wb, const char *pathname, int flags )
{
     int    ret, fd ;
     char  *buff_data_sys ;
     int    local_rank ;

     mfs_print(DBG_INFO, "Client[%d]: dbmopen('%s', %d)\n", mfs_comm_get_rank(wb), pathname, flags) ;

     // Initialize...
     ret = 0 ;
     buff_data_sys = NULL ;
     local_rank    = mfs_comm_get_rank(wb) ;

     // Get filename
     ret = base_str_prepare_pathname(&buff_data_sys, wb->node_url->file, local_rank, strlen(pathname)) ;
     if (ret < 0) {
         mfs_print(DBG_ERROR, "Client[%d]: base_str_prepare_pathname(%d) fails :-(\n", mfs_comm_get_rank(wb), strlen(pathname)) ;
     }
     strcat(buff_data_sys, pathname) ;

     // Open file
     mfs_print(DBG_INFO, "Client[%d]: dbmopen('%s', %d)\n", mfs_comm_get_rank(wb), buff_data_sys, flags) ;

     ret = mfs_dbm_open(&fd, DBM_USE_GDBM, buff_data_sys, flags) ;
     if (ret < 0) {
	 mfs_print(DBG_WARNING, "Client[%d]: file '%s' not opened :-(\n", mfs_comm_get_rank(wb), pathname) ;
	 return -1 ;
     }

     // free filename buffer
     ret = mfs_free(&buff_data_sys) ;
     if (ret < 0) {
         mfs_print(DBG_WARNING, "Client[%d]: problem on free :-(\n", mfs_comm_get_rank(wb)) ;
     }

     return fd ;
}

int  clientstub_local_dbmclose ( comm_t *wb, long fd )
{
     mfs_print(DBG_INFO, "Client[%d]: dbmclose(%d)\n", mfs_comm_get_rank(wb), fd) ;

     return mfs_dbm_close(fd) ;
}

int  clientstub_local_dbmstore ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
     mfs_print(DBG_INFO, "Client[%d]: dbmstore(%d, %d, %d)\n", mfs_comm_get_rank(wb), fd, count_key, count_val) ;

     return mfs_dbm_store(fd, buff_key, count_key, buff_val, count_val) ;
}

int  clientstub_local_dbmfetch ( comm_t *wb, long fd, void *buff_key, int count_key, void *buff_val, int *count_val )
{
     mfs_print(DBG_INFO, "Client[%d]: dbmfetch(%d, %d, %d)\n", mfs_comm_get_rank(wb), fd, count_key, *count_val) ;

     return mfs_dbm_fetch(fd, buff_key, count_key, &buff_val, count_val) ;
}

int  clientstub_local_dbmdelete ( comm_t *wb, long fd, void *buff_key, int count_key )
{
     mfs_print(DBG_INFO, "Client[%d]: dbmdelete(%d, %d)\n", mfs_comm_get_rank(wb), fd, count_key) ;

     return mfs_dbm_delete(fd, buff_key, count_key) ;
}

