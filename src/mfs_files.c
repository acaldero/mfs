
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


#include "mfs_files.h"


/*
 *  Auxiliar functions
 */

            int mfs_file_hash_neltos = 1024 ;
         file_t mfs_file_hash_eltos[1024] ;
pthread_mutex_t mfs_file_mutex = PTHREAD_MUTEX_INITIALIZER ;

int mfs_file_find_free ( void )
{
    pthread_mutex_lock(&mfs_file_mutex) ;
    for (int i=0; i<mfs_file_hash_neltos; i++)
    {
	 if (0 == mfs_file_hash_eltos[i].been_used)
	 {
             memset(&(mfs_file_hash_eltos[i]), 0, sizeof(file_t)) ;
	     mfs_file_hash_eltos[i].been_used = 1 ;
	     mfs_file_hash_eltos[i].file_fd   = i ;
             pthread_mutex_unlock(&mfs_file_mutex) ;
	     return i ;
	 }
    }

    pthread_mutex_unlock(&mfs_file_mutex) ;
    return -1 ;
}

void mfs_file_set_free ( int fd )
{
    pthread_mutex_lock(&mfs_file_mutex) ;
     mfs_file_hash_eltos[fd].been_used = 0 ;
     mfs_file_hash_eltos[fd].file_fd   = -1 ;
    pthread_mutex_unlock(&mfs_file_mutex) ;
}


/*
 *  File System API: descriptors
 */

long  mfs_file_fd2long ( int fd )
{
    file_t *fh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_file_hash_neltos) ) {
	return -1 ;
    }

    // Return file descriptor
    fh = &(mfs_file_hash_eltos[fd]) ;
    return fh->file_fd ;
}

int mfs_file_long2fd ( int *fd, long fref, int file_protocol )
{
    file_t *fh ;

    // Check params...
    if ( (fref < 0) || (fref >= mfs_file_hash_neltos) ) {
	return -1 ;
    }

    // Return OK
    (*fd) = fref ;
    return 1 ;
}

int  mfs_file_stats_show ( int fd, char *prefix )
{
    file_t *fh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_file_hash_neltos) ) {
	return -1 ;
    }

    fh = &(mfs_file_hash_eltos[fd]) ;
    if (fh->been_used != 1) {
	return -1 ;
    }

    // Print stats...
    printf("%s: File:\n",           prefix) ;
    printf("%s: + been_used=%d\n",  prefix, fh->been_used) ;
    printf("%s: + file_fd=%d\n",    prefix, fh->file_fd) ;
    printf("%s: + protocol=%s\n",   prefix, fh->file_protocol_name) ;
    printf("%s:   + posix_fd=%d\n", prefix, fh->posix_fd) ;
    printf("%s:   + mpiio_fd=%d\n", prefix, fh->mpiio_fd) ;
    printf("%s: + offset=%ld\n",    prefix, fh->offset) ;
    printf("%s: + # read=%ld\n",    prefix, fh->n_read_req) ;
    printf("%s: + # write=%ld\n",   prefix, fh->n_write_req) ;

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int  mfs_file_init ( void )
{
    int  ret ;

    // initialize all protocols
    ret = mfs_file_posix_init() ;
    ret = mfs_file_red_init() ;

    // Return OK/KO
    return ret ;
}

int  mfs_file_finalize ( void )
{
    int  ret ;

    // finalize all protocols
    ret = mfs_file_posix_finalize() ;
    ret = mfs_file_red_finalize() ;

    // Return OK/KO
    return ret ;
}

int  mfs_file_open ( int *fd, int file_protocol, const char *path_name, int flags )
{
    int    ret ;
    file_t *fh ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    (*fd) = ret = mfs_file_find_free() ;
    if (ret < 0) {
	return -1 ;
    }

    // Open file
    fh = &(mfs_file_hash_eltos[ret]) ;
    fh->file_protocol = file_protocol ;
    switch (fh->file_protocol)
    {
        case FILE_USE_POSIX:
             fh->file_protocol_name = "POSIX" ;
             fh->posix_fd = (long)mfs_file_posix_open(path_name, flags) ;
             if (fh->posix_fd < 0) {
    	         mfs_print(DBG_INFO, "[FILE]: ERROR on open(path_name='%s', flags=%d, mode=0755)\n", path_name, flags) ;
	         return -1 ;
             }
             break ;

        case FILE_USE_MPI_IO:
             fh->file_protocol_name = "MPI-IO" ;
	     ret = MPI_File_open(MPI_COMM_SELF, path_name,
                                 MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &(fh->mpiio_fd));
	     if (ret != MPI_SUCCESS) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on open('%s') file.\n", path_name) ;
	         return -1 ;
	     }
             break ;

        case FILE_USE_REDIS:
             ret = mfs_file_red_open(&(fh->redis_ctxt), &(fh->redis_key), path_name) ;
             if (ret < 0) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on open('%s') file.\n", path_name) ;
	         return -1 ;
             }
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_protocol(%d).\n", fh->file_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    return 1 ;
}

int   mfs_file_close ( int fd )
{
    int ret ;
    file_t *fh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_file_hash_neltos) ) {
	return -1 ;
    }

    fh = &(mfs_file_hash_eltos[fd]) ;
    if (fh->been_used != 1) {
	return -1 ;
    }

    // Close file
    switch (fh->file_protocol)
    {
        case FILE_USE_POSIX:
             ret = mfs_file_posix_close(fh->posix_fd) ;
             break ;

        case FILE_USE_MPI_IO:
	     ret = MPI_File_close(&(fh->mpiio_fd)) ;
             break ;

        case FILE_USE_REDIS:
             ret = mfs_file_red_close(fh->redis_ctxt, &(fh->redis_key)) ;
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_protocol(%d).\n", fh->file_protocol) ;
	     return -1 ;
             break ;
    }

    // Return OK
    mfs_file_set_free(fd) ;
    return 1 ;
}

int   mfs_file_read  ( int  fd, void *buff_data, int count )
{
    int ret = -1 ;
    file_t *fh ;

    // Check params...
    if ( (fd < 0) || (fd >= mfs_file_hash_neltos) ) {
	return -1 ;
    }

    fh = &(mfs_file_hash_eltos[fd]) ;
    if (fh->been_used != 1) {
	return -1 ;
    }

    // Read from file...
    switch (fh->file_protocol)
    {
        case FILE_USE_POSIX:
             ret = mfs_file_posix_read(fh->posix_fd, buff_data, count) ;
             if (ret < 0) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", count, fh->posix_fd) ;
	         return -1 ;
             }
             break ;

        case FILE_USE_MPI_IO:
             MPI_Status status ;
	     ret = MPI_File_read(fh->mpiio_fd, buff_data, count, MPI_CHAR, &status) ;
	     if (ret != MPI_SUCCESS) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", count, fh->mpiio_fd) ;
	         return -1 ;
	     }

	     MPI_Get_count(&status, MPI_INT, &ret);
             break ;

        case FILE_USE_REDIS:
             ret = mfs_file_red_read(fh->redis_ctxt, fh->redis_key, &(fh->offset), buff_data, count) ;
             if (ret < 0) {
    	         mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from inmemory '%p'\n", count, fh->redis_ctxt) ;
	         return -1 ;
             }
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_protocol(%d).\n", fh->file_protocol) ;
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
    if ( (fd < 0) || (fd >= mfs_file_hash_neltos) ) {
	return -1 ;
    }

    fh = &(mfs_file_hash_eltos[fd]) ;
    if (fh->been_used != 1) {
	return -1 ;
    }

    // Write into file...
    switch (fh->file_protocol)
    {
        case FILE_USE_POSIX:
             ret = mfs_file_posix_write(fh->posix_fd, buff_data, count) ;
             if (ret < 0) {
    	         mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes to file '%d'\n", count, fh->posix_fd) ;
	         return -1 ;
             }
             break ;

        case FILE_USE_MPI_IO:
	     MPI_Status status ;
	     ret = MPI_File_write(fh->mpiio_fd, buff_data, count, MPI_CHAR, &status) ;
	     if (ret != MPI_SUCCESS) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes to file '%d'\n", count, fh->mpiio_fd) ;
	         return -1 ;
	     }

	     MPI_Get_count(&status, MPI_INT, &ret);
             break ;

        case FILE_USE_REDIS:
             ret = mfs_file_red_write(fh->redis_ctxt, fh->redis_key, &(fh->offset), buff_data, count) ;
             if (ret < 0) {
    	         mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes to inmemory '%p'\n", count, fh->redis_ctxt) ;
	         return -1 ;
             }
             break ;

        default:
	     mfs_print(DBG_INFO, "[FILE]: ERROR on file_protocol(%d).\n", fh->file_protocol) ;
	     return -1 ;
             break ;
    }

    // Stats...
    (fh->n_write_req) ++ ;

    // Return number_bytes / KO
    return ret ;
}

