
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


#include "mfs_files.h"


/*
 *  File System API: descriptors
 */

descriptor_t mfs_files_des ;


long  mfs_file_fd2long ( int fd )
{
    file_t *fh ;

    // Get file handler
    fh = (file_t *)mfs_descriptor_fd2fh(&mfs_files_des, fd, sizeof(file_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Return file descriptor
    return (long)fd ;
}

int mfs_file_long2fd ( int *fd, long fref )
{
    file_t *fh ;

    // Get file handler
    fh = (file_t *)mfs_descriptor_fd2fh(&mfs_files_des, fref, sizeof(file_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Get associated file descriptor (itself)
    (*fd) = fref ;

    // Return OK
    return 1 ;
}

int  mfs_file_stats_show ( int fd, char *prefix )
{
    file_t *fh ;

    // Get file handler
    fh = (file_t *)mfs_descriptor_fd2fh(&mfs_files_des, fd, sizeof(file_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Print stats...
    printf("%s: File:\n",            prefix) ;
    printf("%s: + been_used=1\n",    prefix) ;
    printf("%s: + file_fd=%d\n",     prefix, fd) ;
    printf("%s: + protocol=%s\n",    prefix, fh->file_backend_name) ;
    printf("%s:   + posix_fd=%d\n",  prefix, fh->posix_fd) ;
    printf("%s:   + mpiio_fd=%d\n",  prefix, fh->mpiio_fd) ;
    printf("%s: + offset=%ld\n",     prefix, fh->offset) ;
    printf("%s: + # read=%ld\n",     prefix, fh->n_read_req) ;
    printf("%s: + # write=%ld\n",    prefix, fh->n_write_req) ;

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int  mfs_file_init ( void )
{
    int  ret ;

    // initialize descriptors
    ret = mfs_descriptor_init(&mfs_files_des, 1024, sizeof(file_t)) ;
    if (ret < 0) {
	return -1 ;
    }

    // initialize all protocols
    ret = mfs_file_posix_init() ;
    if (ret < 0) {
	return -1 ;
    }

    ret = mfs_file_mpi_init() ;
    if (ret < 0) {
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int  mfs_file_finalize ( void )
{
    int  ret ;

    // finalize all protocols
    ret = mfs_file_posix_finalize() ;
    if (ret < 0) {
	return -1 ;
    }

    ret = mfs_file_mpi_finalize() ;
    if (ret < 0) {
	return -1 ;
    }

    // finalize descriptors
    ret = mfs_descriptor_finalize(&mfs_files_des) ;
    if (ret < 0) {
	return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int  mfs_file_open ( int *fd, int file_backend, const char *path_name, int flags )
{
    int    ret ;
    file_t *fh ;

    // Check params...
    NULL_PRT_MSG_RET_VAL(fd, "[FILE] NULL fd :-/", -1) ;

    // Get fd
    (*fd) = ret = mfs_descriptor_find_free(&mfs_files_des, sizeof(file_t)) ;
    if (ret < 0) {
	return -1 ;
    }

    // Open file
    fh = (file_t *)mfs_descriptor_fd2fh(&mfs_files_des, ret, sizeof(file_t)) ;
    fh->file_backend = file_backend ;
    switch (fh->file_backend)
    {
        case FILE_USE_POSIX:
             fh->file_backend_name = "POSIX" ;
             ret = (long)mfs_file_posix_open(&(fh->posix_fd), path_name, flags) ;
             break ;

        case FILE_USE_MPI_IO:
             fh->file_backend_name = "MPI-IO" ;
             ret = mfs_file_mpi_open(&(fh->mpiio_fd), path_name) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_backend(%d).\n", fh->file_backend) ;
	     return -1 ;
             break ;
    }

    // Get back descriptor
    if (ret < 0) {
        mfs_descriptor_set_free(&mfs_files_des, *fd) ;
        *fd = -1 ;
    }

    // Return OK
    return ret ;
}

int   mfs_file_close ( int fd )
{
    int ret ;
    file_t *fh ;

    // Check params...
    fh = (file_t *)mfs_descriptor_fd2fh(&mfs_files_des, fd, sizeof(file_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Close file
    switch (fh->file_backend)
    {
        case FILE_USE_POSIX:
             ret = mfs_file_posix_close(fh->posix_fd) ;
             break ;

        case FILE_USE_MPI_IO:
             ret = mfs_file_mpi_close(&(fh->mpiio_fd)) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_backend(%d).\n", fh->file_backend) ;
	     return -1 ;
             break ;
    }

    // Get back descriptor
    mfs_descriptor_set_free(&mfs_files_des, fd) ;

    // Return OK
    return ret ;
}

int   mfs_file_read  ( int  fd, void *buff_data, int count )
{
    int ret = -1 ;
    file_t *fh ;

    // Check params...
    fh = (file_t *)mfs_descriptor_fd2fh(&mfs_files_des, fd, sizeof(file_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Read from file...
    switch (fh->file_backend)
    {
        case FILE_USE_POSIX:
             ret = mfs_file_posix_read(fh->posix_fd, buff_data, count) ;
             break ;

        case FILE_USE_MPI_IO:
             ret = mfs_file_mpi_read(fh->mpiio_fd, buff_data, count) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_backend(%d).\n", fh->file_backend) ;
	     return -1 ;
             break ;
    }

    // Stats...
    (fh->n_read_req) ++ ;

    // Return number_bytes readed
    return ret ;
}

int   mfs_file_write  ( int  fd, void *buff_data, int count )
{
    int ret = -1 ;
    file_t *fh ;

    // Check params...
    fh = (file_t *)mfs_descriptor_fd2fh(&mfs_files_des, fd, sizeof(file_t)) ;
    if (NULL == fh) {
	return -1 ;
    }

    // Write into file...
    switch (fh->file_backend)
    {
        case FILE_USE_POSIX:
             ret = mfs_file_posix_write(fh->posix_fd, buff_data, count) ;
             break ;

        case FILE_USE_MPI_IO:
             ret = mfs_file_mpi_write(fh->mpiio_fd, buff_data, count) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_backend(%d).\n", fh->file_backend) ;
	     return -1 ;
             break ;
    }

    // Stats...
    (fh->n_write_req) ++ ;

    // Return number_bytes / KO
    return ret ;
}

