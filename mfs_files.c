
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

int  server_files_open ( long *fd, int file_protocol, const char *path_name, int flags )
{ 
    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    if (USE_POSIX == file_protocol)
    {
        // Open file
        *fd = (long)open(path_name, flags, 0755) ;
        if (*fd < 0) {
    	    mfs_print(DBG_INFO, "ERROR: on open(path_name='%s', flags=%d, mode=0755)\n", path_name, flags) ;
        }

        // Return OK
	return 1 ;
    }
    if (USE_MPI_IO == file_protocol)
    {
	MPI_File fh ;

        // Open file
	int ret = MPI_File_open(MPI_COMM_SELF, path_name, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "ERROR: open('%s') file.\n", path_name) ;
	    return -1 ;
	}

	*fd = (long)fh ;

	// Return OK
	return 1 ;
    }

    // Return KO
    return -1 ;
}

int   server_files_close ( long fd, int file_protocol )
{
    int ret = -1 ;

    // Close file
    if (USE_POSIX == file_protocol)
    {
        // close file
        ret = close(fd) ;
    }
    if (USE_MPI_IO == file_protocol)
    {
	MPI_File fh = (MPI_File)fd ;
	ret = MPI_File_close(&(fh)) ;
    }

    // Return OK/KO
    return ret ;
}

int   server_files_read  ( long fd, int file_protocol, void *buff_data, int count )
{
    int ret = -1 ;

    // Read from file...
    if (USE_POSIX == file_protocol)
    {
        ret = read(fd, buff_data, count) ;
        if (ret < 0) {
	    mfs_print(DBG_INFO, "ERROR: read %d bytes from file '%d'\n", count, fd) ;
        }
    }
    if (USE_MPI_IO == file_protocol)
    {
	MPI_File fh = (MPI_File)fd ;
	MPI_Status status ;

	ret = MPI_File_read(fh, buff_data, count, MPI_CHAR, &status) ;
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "ERROR: read %d bytes from file '%d'\n", count, fh) ;
	    return -1 ;
	}

	ret = count ;
    }

    // Return number_bytes readed
    return ret ;
}

int   server_files_write ( long fd, int file_protocol, void *buff_data, int count )
{
    int ret = -1 ;

    // Write into file...
    if (USE_POSIX == file_protocol)
    {
        ret = write(fd, buff_data, count) ;
        if (ret < 0) {
    	    mfs_print(DBG_INFO, "ERROR: write %d bytes from file '%d'\n", count, fd) ;
        }
    }
    if (USE_MPI_IO == file_protocol)
    {
	MPI_File fh = (MPI_File)fd ;
	MPI_Status status ;

	ret = MPI_File_write(fh, buff_data, count, MPI_CHAR, &status) ;
	if (ret != MPI_SUCCESS) {
	    mfs_print(DBG_INFO, "ERROR: write %d bytes from file '%d'\n", count, fh) ;
	    return -1 ;
	}

	ret = count ;
    }

    // Return number_bytes / KO
    return ret ;
}

void *server_files_mmap   ( void *addr, size_t size, int protect, int flags, long filedes, off_t offset )
{
    void *ptr = NULL ;

    ptr = mmap(addr, size, protect, flags, filedes, offset) ;
    if (ptr == MAP_FAILED) {
    	mfs_print(DBG_INFO, "ERROR: mapping failed\n") ;
	return NULL ;
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

