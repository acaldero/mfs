
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
#include "mfs_api.h"

#define STR_SIZE 1024

int main_test_file ( mfs_t *wb )
{
    int  ret ;
    long fd ;
    char str[STR_SIZE] ;
    int  rank ;

    rank = mfs_api_get_rank(wb) ;

    // write("hello world")
    strcpy(str, "hello world") ;

    printf("Client[%d]: creat(...) + write(...) + close(...)\n", rank) ;
    fd = mfs_api_open(wb, "test1.txt", O_WRONLY | O_CREAT | O_TRUNC) ;
    mfs_api_write(wb, fd, str, strlen(str)) ;
    mfs_api_close(wb, fd) ;

    // read("...")
    printf("Client[%d]: open(...) + read(...) + close(...)\n", rank) ;
    fd = mfs_api_open(wb, "test1.txt", O_RDONLY) ;
     mfs_api_read(wb, fd, str, STR_SIZE) ;
    mfs_api_close(wb, fd) ;

    return 0;
}

int main_test_dbm ( mfs_t *wb )
{
    int    ret ;
    long   fd ;
    int    n_servers, rank ;
    char   str1[STR_SIZE] ;
    char   str2[STR_SIZE] ;
    int    str1_len ;
    int    str2_len ;

    rank = mfs_api_get_rank(wb) ;

    // dbmstore("m1", "hello world")
    n_servers = mfs_comm_get_nservers(wb) ;
    rank      = mfs_api_get_rank(wb) ;
    if (rank < n_servers)
    {
        strcpy(str1, "m1") ;
        strcpy(str2, "hello world") ;
        str1_len = strlen(str1) + 1 ;
        str2_len = strlen(str2) + 1 ;

        printf("Client[%d]: dbmopen(...) + dbmstore(...) + dbmclose(...)\n", rank) ;
        fd = mfs_api_dbmopen(wb, "test2.txt", GDBM_WRITER | GDBM_WRCREAT) ;
        if (fd < 0) return -1 ;
        mfs_api_dbmstore(wb, fd, str1, str1_len, str2, str2_len) ;
        mfs_api_dbmclose(wb, fd) ;
    }

    // dbmfetch("m1")
    strcpy(str1, "m1") ;
    strcpy(str2, "") ;
    str1_len = strlen(str1) + 1 ;
    str2_len = STR_SIZE ;

    printf("Client[%d]: dbmopen(...) + dbmfetch(...) + dbmclose(...)\n", rank) ;
    fd = mfs_api_dbmopen(wb, "test2.txt", GDBM_READER) ;
    if (fd < 0) return -1 ;
    mfs_api_dbmfetch(wb, fd, str1, str1_len, &str2, &str2_len) ;
    mfs_api_dbmclose(wb, fd) ;

    // dbmdelete("m1")
    if (rank < n_servers)
    {
        strcpy(str1, "m1") ;
        str1_len = strlen(str1) + 1 ;

        printf("Client[%d]: dbmopen(...) + dbmdelete(...) + dbmclose(...)\n", rank) ;
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
    mfs_t    wb ;

    // Welcome...
    printf("\n"
 	   " mfs_client\n"
	   " ----------\n") ;

    // Initialize...
    mfs_print(DBG_INFO, "Client[%d]: initializing...\n", -1) ;
    ret = mfs_api_init(&wb, &params, &argc, &argv) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "Client[%d]: mfs_api_init fails :-(\n", -1) ;
        exit(-1) ;
    }

    // Open partition...
    if (ret >= 0)
    {
        ret = mfs_api_open_partition(&wb, &params, params.conf_fname) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: mfs_api_open_partition fails :-(\n", -1) ;
        }
    }

    // simple main 1...
    if (ret >= 0)
    {
        ret = main_test_file(&wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: main_test_file fails :-(\n", -1) ;
        }
    }

    // simple main 2...
    if (ret >= 0)
    {
        ret = main_test_dbm(&wb) ;
        if (ret < 0) {
            mfs_print(DBG_ERROR, "Client[%d]: main_test_dbm fails :-(\n", -1) ;
        }
    }

    // Close partition...
    //if (ret >= 0)
    {
        mfs_api_close_partition(&wb, &params) ;
    }

    // Finalize...
    printf("Client[%d]: finalize...\n", -1) ;
    mfs_api_finalize(&wb, &params) ;

    // Return OK/KO
    return ret ;
}

