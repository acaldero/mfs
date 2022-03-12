
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


#include "mfs_dbm.h"


/*
 *  File System API: descriptors
 */

descriptor_t mfs_dbm_des ;


long  mfs_dbm_fd2long ( int fd )
{
    dbm_t *fh ;

    // Get file handler
    fh = (dbm_t *)mfs_descriptor_fd2fh(&(mfs_dbm_des), fd, sizeof(dbm_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Return file descriptor
    return (long)fd ;
}

int mfs_dbm_long2fd ( int *fd, long fref, int dbm_backend )
{
    dbm_t *fh ;

    // Get file handler
    fh = (dbm_t *)mfs_descriptor_fd2fh(&mfs_dbm_des, fref, sizeof(dbm_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Get associated file descriptor (itself)
    (*fd) = fref ;

    // Return OK
    return 1 ;
}

int  mfs_dbm_stats_show ( int fd, char *prefix )
{
    dbm_t *fh ;

    // Get dbm handler
    fh = (dbm_t *)mfs_descriptor_fd2fh(&mfs_dbm_des, fd, sizeof(dbm_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Print stats...
    printf("%s: File:\n",           prefix) ;
    printf("%s: + been_used=1\n",   prefix) ;
    printf("%s: + dbm_fd=%d\n",     prefix, fd) ;
    printf("%s: + protocol=%s\n",   prefix, fh->dbm_backend_name) ;
    printf("%s:   + ndbm_fd=%d\n",  prefix, fh->ndbm_fd) ;
    printf("%s: + offset=%ld\n",    prefix, fh->offset) ;
    printf("%s: + # read=%ld\n",    prefix, fh->n_read_req) ;
    printf("%s: + # write=%ld\n",   prefix, fh->n_write_req) ;

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int  mfs_dbm_init ( void )
{
    int  ret ;

    // initialize descriptors
    ret = mfs_descriptor_init(&mfs_dbm_des, 1024, sizeof(dbm_t)) ;
    if (ret < 0) {
	return -1 ;
    }

    // initialize all protocols
    ret = mfs_dbm_ndbm_init() ;
    if (ret < 0) {
	return -1 ;
    }

    /*
    ret = mfs_dbm_redis_init() ;
    if (ret < 0) {
	return -1 ;
    }
    */

    // Return OK/KO
    return ret ;
}

int  mfs_dbm_finalize ( void )
{
    int  ret ;

    // finalize all protocols
    ret = mfs_dbm_ndbm_finalize() ;
    if (ret < 0) {
	return -1 ;
    }

    /*
    ret = mfs_dbm_redis_finalize() ;
    if (ret < 0) {
	return -1 ;
    }
    */

    // finalize descriptors
    ret = mfs_descriptor_finalize(&mfs_dbm_des) ;
    if (ret < 0) {
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int  mfs_dbm_open ( int *fd, int dbm_backend, const char *path_name, int flags )
{
    int    ret ;
    dbm_t *fh ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    (*fd) = ret = mfs_descriptor_find_free(&mfs_dbm_des, sizeof(dbm_t)) ;
    if (ret < 0) {
	return -1 ;
    }

    // Open file
    fh = (dbm_t *)mfs_descriptor_fd2fh(&mfs_dbm_des, ret, sizeof(dbm_t)) ;
    fh->dbm_backend = dbm_backend ;
    switch (fh->dbm_backend)
    {
        case DBM_USE_NDBM:
             fh->dbm_backend_name = "NDBM" ;
             ret = (long)mfs_dbm_ndbm_open(&(fh->ndbm_fd), path_name, flags) ;
             break ;

	/*
        case DBM_USE_REDIS:
             fh->dbm_backend_name = "REDIS" ;
             ret = mfs_dbm_redis_open(&(fh->redis_ctxt), &(fh->redis_key), path_name) ;
             break ;
	*/

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on dbm_backend(%d).\n", fh->dbm_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return ret ;
}

int   mfs_dbm_close ( int fd )
{
    int ret ;
    dbm_t *fh ;

    // Check params...
    fh = (dbm_t *)mfs_descriptor_fd2fh(&mfs_dbm_des, fd, sizeof(dbm_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Close file
    switch (fh->dbm_backend)
    {
        case DBM_USE_NDBM:
             ret = mfs_dbm_ndbm_close(fh->ndbm_fd) ;
             break ;

	/*
        case DBM_USE_REDIS:
             ret = mfs_dbm_redis_close(fh->redis_ctxt, &(fh->redis_key)) ;
             break ;
	*/

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on dbm_backend(%d).\n", fh->dbm_backend) ;
	     return -1 ;
             break ;
    }

    // Return OK
    mfs_descriptor_set_free(&mfs_dbm_des, fd) ;
    return ret ;
}

int   mfs_dbm_store  ( int  fd, void *buff_key, int count_key, void *buff_val, int count_val )
{
    int ret = -1 ;
    dbm_t *fh ;

    // Check params...
    fh = (dbm_t *)mfs_descriptor_fd2fh(&mfs_dbm_des, fd, sizeof(dbm_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Read from file...
    switch (fh->dbm_backend)
    {
        case DBM_USE_NDBM:
             ret = mfs_dbm_ndbm_store(fh->ndbm_fd, buff_key, count_key, buff_val, count_val) ;
             break ;

	/*
        case DBM_USE_REDIS:
             ret = mfs_dbm_redis_store(fh->redis_ctxt, fh->redis_key, &(fh->offset), buff_data, count) ;
             break ;
	*/

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on dbm_backend(%d).\n", fh->dbm_backend) ;
	     return -1 ;
             break ;
    }

    // Stats...
    (fh->n_read_req) ++ ;

    // Return number_bytes readed
    return ret ;
}

int   mfs_dbm_fetch  ( int  fd, void *buff_key, int count_key, void **buff_val, int *count_val )
{
    int ret = -1 ;
    dbm_t *fh ;

    // Check params...
    fh = (dbm_t *)mfs_descriptor_fd2fh(&mfs_dbm_des, fd, sizeof(dbm_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Write into file...
    switch (fh->dbm_backend)
    {
        case DBM_USE_NDBM:
             ret = mfs_dbm_ndbm_fetch(fh->ndbm_fd, buff_key, count_key, buff_val, count_val) ;
             break ;

	/*
        case DBM_USE_REDIS:
             ret = mfs_dbm_redis_fetch(fh->redis_ctxt, fh->redis_key, &(fh->offset), buff_data, count) ;
             break ;
	*/

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on dbm_backend(%d).\n", fh->dbm_backend) ;
	     return -1 ;
             break ;
    }

    // Stats...
    (fh->n_write_req) ++ ;

    // Return number_bytes / KO
    return ret ;
}

int   mfs_dbm_delete  ( int  fd, void *buff_key, int count_key )
{
    int ret = -1 ;
    dbm_t *fh ;

    // Check params...
    fh = (dbm_t *)mfs_descriptor_fd2fh(&mfs_dbm_des, fd, sizeof(dbm_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Write into file...
    switch (fh->dbm_backend)
    {
        case DBM_USE_NDBM:
             ret = mfs_dbm_ndbm_delete(fh->ndbm_fd, buff_key, count_key) ;
             break ;

	/*
        case DBM_USE_REDIS:
             ret = mfs_dbm_redis_delete(fh->redis_ctxt, fh->redis_key, &(fh->offset), buff_data, count) ;
             break ;
	*/

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on dbm_backend(%d).\n", fh->dbm_backend) ;
	     return -1 ;
             break ;
    }

    // Stats...
    (fh->n_write_req) ++ ;

    // Return number_bytes / KO
    return ret ;
}

