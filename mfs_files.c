
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


long  server_files_fd2int ( file_t *fd )
{ 
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    if (USE_POSIX  == fd->file_protocol) {
        return (long) fd->fd ;
    }
    if (USE_MPI_IO == fd->file_protocol) {
        return (long) fd->fh ;
    }

    // Return KO
    return -1 ;
} 

file_t server_files_int2fd ( long fint, int file_protocol )
{ 
    file_t fd ;

    fd.file_protocol = file_protocol ;
    fd.fd = 0 ;
    fd.fh = NULL ;

    if (USE_POSIX  == fd.file_protocol) {
        fd.fd =      (int)fint ;
    }
    if (USE_MPI_IO == fd.file_protocol) {
        fd.fh = (MPI_File)fint ;
    }

    return fd ;
} 


/*
 *  File System API 
 */

int  server_files_open ( file_t *fd, int file_protocol, const char *path_name, int flags )
{ 
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    fd->file_protocol = file_protocol ;
    fd->fd = 0 ;
    fd->fh = NULL ;

    if (USE_POSIX == fd->file_protocol)
    {
        // Open file
        fd->fd = open(path_name, flags, 0755) ;
        if (fd->fd < 0) {
    	    mfs_print(DBG_INFO, "ERROR: on open(path_name='%s', flags=%d, mode=0755)\n", path_name, flags) ;
        }

        // Return OK
	return 1 ;
    }
    if (USE_MPI_IO == fd->file_protocol)
    {
        // Open file
	int ret = MPI_File_open(MPI_COMM_SELF, path_name, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &(fd->fh));
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "ERROR: open('%s') file.\n", path_name) ;
	    return -1 ;
	}

	// Return OK
	return 1 ;
    }

    // Return KO
    return -1 ;
}

int   server_files_close ( file_t *fd )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return ret ;
    }

    // Close file
    if (USE_POSIX == fd->file_protocol)
    {
        // close file
        ret = close(fd->fd) ;
    }
    if (USE_MPI_IO == fd->file_protocol)
    {
	ret = MPI_File_close(&(fd->fh)) ;
    }

    // Return OK/KO
    return ret ;
}

int   server_files_read  ( file_t *fd, void *buff_data, int count )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return ret ;
    }

    // Read from file...
    if (USE_POSIX == fd->file_protocol)
    {
        ret = read(fd->fd, buff_data, count) ;
        if (ret < 0) {
	    mfs_print(DBG_INFO, "ERROR: read %d bytes from file '%d'\n", count, fd->fd) ;
        }
    }
    if (USE_MPI_IO == fd->file_protocol)
    {
	MPI_Status status ;

	ret = MPI_File_read(fd->fh, buff_data, count, MPI_CHAR, &status) ;
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "ERROR: read %d bytes from file '%d'\n", count, fd->fh) ;
	    return -1 ;
	}

	ret = count ;
    }

    // Return number_bytes readed
    return ret ;
}

int   server_files_write ( file_t *fd, void *buff_data, int count )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return ret ;
    }

    // Write into file...
    if (USE_POSIX == fd->file_protocol)
    {
        ret = write(fd->fd, buff_data, count) ;
        if (ret < 0) {
    	    mfs_print(DBG_INFO, "ERROR: write %d bytes from file '%d'\n", count, fd->fd) ;
        }
    }
    if (USE_MPI_IO == fd->file_protocol)
    {
	MPI_Status status ;

	ret = MPI_File_write(fd->fh, buff_data, count, MPI_CHAR, &status) ;
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "ERROR: write %d bytes from file '%d'\n", count, fd->fh) ;
	    return -1 ;
	}

	ret = count ;
    }

    // Return number_bytes / KO
    return ret ;
}

void *server_files_mmap   ( void *addr, size_t size, int protect, int flags, file_t *filedes, off_t offset )
{
    void *ptr = NULL ;

    if (USE_POSIX == filedes->file_protocol)
    {
        ptr = mmap(addr, size, protect, flags, filedes->fd, offset) ;
        if (ptr == MAP_FAILED) {
    	    mfs_print(DBG_INFO, "ERROR: mapping failed\n") ;
	    return NULL ;
        }
    }

    return ptr ;
}

int    server_files_munmap ( void *addr, size_t size )
{
    int err ;

    err = munmap(addr, size);
    if (err != 0) {
	mfs_print(DBG_INFO, "ERROR: UnMapping failed\n") ;
	return -1;
    }

    // Return OK
    return 1 ;
}

