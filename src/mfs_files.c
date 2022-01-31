
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
 *  File System API
 */

long  mfs_file_fd2long ( file_t *fd )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    if (FILE_USE_POSIX  == fd->file_protocol) {
        return (long) fd->posix_fd ;
    }
    if (FILE_USE_MPI_IO == fd->file_protocol) {
        return (long) fd->mpiio_fd ;
    }

    // Return KO
    return -1 ;
}

int mfs_file_long2fd ( file_t *fd, long fref, int file_protocol )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    fd->file_protocol = file_protocol ;
    fd->posix_fd      = 0 ;
    fd->mpiio_fd      = 0 ;
    fd->n_read_req    = 0 ;
    fd->n_write_req   = 0 ;

    if (FILE_USE_POSIX  == fd->file_protocol) {
        fd->posix_fd =      (int)fref ;
    }
    if (FILE_USE_MPI_IO == fd->file_protocol) {
        fd->mpiio_fd = (MPI_File)fref ;
    }

    // Return OK
    return 1 ;
}

int  mfs_file_stats_show ( file_t *fd, char *prefix )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    char *proto = "unknow" ;
    if (FILE_USE_POSIX == fd->file_protocol)
        proto = "POSIX" ;
    if (FILE_USE_MPI_IO == fd->file_protocol)
        proto = "MPI-IO" ;

    // Print stats...
    printf("%s: File:\n",           prefix) ;
    printf("%s: + protocol=%s\n",   prefix, proto) ;
    printf("%s: + posix_fd=%d\n",   prefix, fd->posix_fd) ;
    printf("%s: + mpiio_fd=%d\n",   prefix, fd->mpiio_fd) ;
    printf("%s: + # read=%d\n",     prefix, fd->n_read_req) ;
    printf("%s: + # write=%d\n",    prefix, fd->n_write_req) ;

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int  mfs_file_open ( file_t *fd, int file_protocol, const char *path_name, int flags )
{
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    fd->file_protocol = file_protocol ;
    fd->posix_fd      = 0 ;
    fd->mpiio_fd      = 0 ;
    fd->n_read_req    = 0 ;
    fd->n_write_req   = 0 ;

    if (FILE_USE_POSIX == fd->file_protocol)
    {
        // Open file
        fd->posix_fd = (long)open(path_name, flags, 0755) ;
        if (fd->posix_fd < 0) {
    	    mfs_print(DBG_INFO, "[FILE]: ERROR on open(path_name='%s', flags=%d, mode=0755)\n", path_name, flags) ;
        }

        // Return OK
	return 1 ;
    }
    if (FILE_USE_MPI_IO == fd->file_protocol)
    {
        // Open file
	int ret = MPI_File_open(MPI_COMM_SELF, path_name, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &(fd->mpiio_fd));
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "[FILE]: ERROR on open('%s') file.\n", path_name) ;
	    return -1 ;
	}

	// Return OK
	return 1 ;
    }

    // Return KO
    return -1 ;
}

int   mfs_file_close ( file_t *fd )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Close file
    if (FILE_USE_POSIX == fd->file_protocol)
    {
        // close file
        ret = close(fd->posix_fd) ;
    }
    if (FILE_USE_MPI_IO == fd->file_protocol)
    {
	ret = MPI_File_close(&(fd->mpiio_fd)) ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_file_read  ( file_t *fd, void *buff_data, int count )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Read from file...
    if (FILE_USE_POSIX == fd->file_protocol)
    {
        ret = read(fd->posix_fd, buff_data, count) ;
        if (ret < 0) {
	    mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", count, fd->posix_fd) ;
	    return -1 ;
        }
    }
    if (FILE_USE_MPI_IO == fd->file_protocol)
    {
	MPI_Status status ;

	ret = MPI_File_read(fd->mpiio_fd, buff_data, count, MPI_CHAR, &status) ;
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", count, fd->mpiio_fd) ;
	    return -1 ;
	}

	ret = count ;
    }

    // Stats...
    fd->n_read_req ++ ;

    // Return number_bytes readed
    return ret ;
}

int   mfs_file_write  ( file_t *fd, void *buff_data, int count )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Write into file...
    if (FILE_USE_POSIX == fd->file_protocol)
    {
        ret = write(fd->posix_fd, buff_data, count) ;
        if (ret < 0) {
    	    mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes from file '%d'\n", count, fd->posix_fd) ;
	    return -1 ;
        }
    }
    if (FILE_USE_MPI_IO == fd->file_protocol)
    {
	MPI_Status status ;

	ret = MPI_File_write(fd->mpiio_fd, buff_data, count, MPI_CHAR, &status) ;
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes from file '%d'\n", count, fd->mpiio_fd) ;
	    return -1 ;
	}

	ret = count ;
    }

    // Stats...
    fd->n_write_req ++ ;

    // Return number_bytes / KO
    return ret ;
}

void *mfs_file_mmap   ( void *addr, size_t size, int protect, int flags, long filedes, off_t offset )
{
    void *ptr = NULL ;

    ptr = mmap(addr, size, protect, flags, filedes, offset) ;
    if (ptr == MAP_FAILED) {
    	mfs_print(DBG_INFO, "[FILE]: ERROR on mapping failed\n") ;
	return NULL ;
    }

    return ptr ;
}

int    mfs_file_munmap ( void *addr, size_t size )
{
    int err ;

    err = munmap(addr, size);
    if (err != 0) {
	mfs_print(DBG_INFO, "[FILE]: ERROR on UnMapping failed\n") ;
	return -1;
    }

    // Return OK
    return 1 ;
}

