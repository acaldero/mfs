
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MPI_PFS.
 *
 *  MPI_PFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MPI_PFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MPI_PFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <stdio.h>
#include "mfs_client_api.h"

#define STR_SIZE 1024

int main_test_file ( params_t *params, comm_t *wb )
{
    int    ret ;
    long   fd ;
    char   str[STR_SIZE] ;

    // Example
    strcpy(str, "hello word") ;

    printf("Client[%d]: creat(...) + write(...) + close(...)\n", wb->rank) ;
    fd = mfs_api_open(wb, "test1.txt", O_WRONLY | O_CREAT | O_TRUNC) ;
    mfs_api_write(wb, fd, str, strlen(str)) ;
    mfs_api_close(wb, fd) ;

    printf("Client[%d]: open(...) + read(...) + close(...)\n", wb->rank) ;
    fd = mfs_api_open(wb, "test1.txt", O_RDONLY) ;
     mfs_api_read(wb, fd, str, STR_SIZE) ;
    mfs_api_close(wb, fd) ;

    return 0;
}

int main_test_dbm ( params_t *params, comm_t *wb )
{
    int    ret ;
    long   fd ;
    char   str1[STR_SIZE] ;
    char   str2[STR_SIZE] ;
    int    str1_len ;
    int    str2_len ;

    MPI_Barrier(MPI_COMM_WORLD) ;

    // dbmstore("m1", "hello world")
    if (wb->rank < wb->n_servers)
    {
        strcpy(str1, "m1") ;
        strcpy(str2, "hello world") ;
        str1_len = strlen(str1) + 1 ;
        str2_len = strlen(str2) + 1 ;

        printf("Client[%d]: dbmopen(...) + dbmstore(...) + dbmclose(...)\n", wb->rank) ;
        fd = mfs_api_dbmopen(wb, "test2.txt", GDBM_WRITER | GDBM_WRCREAT) ;
        if (fd < 0) return -1 ;
        mfs_api_dbmstore(wb, fd, str1, str1_len, str2, str2_len) ;
        mfs_api_dbmclose(wb, fd) ;
    }

    MPI_Barrier(MPI_COMM_WORLD) ;

    // dbmfetch("m1")
    strcpy(str1, "m1") ;
    strcpy(str2, "") ;
    str1_len = strlen(str1) + 1 ;
    str2_len = STR_SIZE ;

    printf("Client[%d]: dbmopen(...) + dbmfetch(...) + dbmclose(...)\n", wb->rank) ;
    fd = mfs_api_dbmopen(wb, "test2.txt", GDBM_READER) ;
    if (fd < 0) return -1 ;
    mfs_api_dbmfetch(wb, fd, str1, str1_len, &str2, &str2_len) ;
    mfs_api_dbmclose(wb, fd) ;

    MPI_Barrier(MPI_COMM_WORLD) ;

    // dbmdelete("m1")
    if (wb->rank < wb->n_servers)
    {
        strcpy(str1, "m1") ;
        str1_len = strlen(str1) + 1 ;

        printf("Client[%d]: dbmopen(...) + dbmdelete(...) + dbmclose(...)\n", wb->rank) ;
        fd = mfs_api_dbmopen(wb, "test2.txt", GDBM_WRITER) ;
        if (fd < 0) return -1 ;
        mfs_api_dbmdelete(wb, fd, str1, str1_len) ;
        mfs_api_dbmclose(wb, fd) ;
    }

    return 0;
}

int main ( int argc, char **argv )
{
    int      ret ;
    params_t params ;
    comm_t   wb ;

    // Welcome...
    printf("\n"
 	   " mfs_client\n"
	   " ----------\n"
	   "\n") ;

    // Get parameters..
    ret = mfs_params_get(&params, &argc, &argv) ;
    if (ret < 0) {
        mfs_params_show_usage() ;
        exit(-1) ;
    }

    printf("Client[%d]: initializing...\n", -1) ;
    mfs_params_show(&params) ;

    // Initialize...
    if (ret >= 0)
    {
        ret = mfs_api_init(&wb, &params) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: mfs_api_init fails :-(", -1) ;
        }
    }

    // simple main 1...
    if (ret >= 0)
    {
        ret = main_test_file(&params, &wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: main_test_file fails :-(", -1) ;
        }
    }

    // simple main 2...
    if (ret >= 0)
    {
        ret = main_test_dbm(&params, &wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: main_test_dbm fails :-(", -1) ;
        }
    }

    // Finalize...
    printf("Client[%d]: finalize...\n", wb.rank) ;
    mfs_api_finalize(&wb, &params) ;

    // Return OK/KO
    return ret ;
}

