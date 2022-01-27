
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
 *  File System API (POSIX)
 */

#ifdef USE_POSIX

	long server_files_open ( const char *pathname, int flags )
	{
	    long fd ;

	    // open file
	    fd = open(pathname, flags, 0755) ;
	    if (fd < 0) {
		mfs_print(DBG_INFO, "ERROR: on open(pathname='%s', flags=%d, mode=0755)\n", pathname, flags) ;
	    }

	    // Return file_descriptor
	    return fd ;
	}

	int server_files_close ( long fd )
	{
	    int ret ;

	    // close file
	    ret = close(fd) ;

	    // Return OK(0)/KO(-1)
	    return ret ;
	}

	int server_files_read ( long fd, void *buff_data, int count )
	{
	    int ret ;

	    // read data
	    ret = read(fd, buff_data, count) ;
	    if (ret < 0) {
		mfs_print(DBG_INFO, "ERROR: read %d bytes from file '%d'\n", count, fd) ;
	    }

	    // Return number_bytes readed
	    return ret ;
	}

	int server_files_write ( long fd, void *buff_data, int count )
	{
	    int ret ;

	    // write data
	    ret = write(fd, buff_data, count) ;
	    if (ret < 0) {
		mfs_print(DBG_INFO, "ERROR: write %d bytes from file '%d'\n", count, fd) ;
	    }

	    // Return number_bytes / KO
	    return ret ;
	}

	void * server_files_mmap ( void *addr, size_t size, int protect, int flags, long filedes, off_t offset )
	{
	    void *ptr ;

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

#endif


/*
 *  File System API (MPI-io)
 */

#ifdef USE_MPI_IO

	long server_files_open ( const char *pathname, int flags )
	{
	    int ret ;
	    MPI_File *fh ;

	    fh = (MPI_File *)malloc(sizeof(MPI_File)) ;
	    if (NULL == fh) {
		return -1 ;
	    }

	    ret = MPI_File_open(MPI_COMM_SELF, pathname, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, fh);
	    if (ret != MPI_SUCCESS) {
		mfs_print(DBG_INFO, "ERROR: open('%s') file.\n", pathname) ;
		free(fh) ;
		return -1 ;
	    }

	    // Return file_descriptor
	    return (long)fh ;
	}

	int server_files_close ( long fd )
	{
	    int ret ;
	    MPI_File *fh ;

	    fh = (MPI_File *)fd ;
	    ret = MPI_File_close(fh) ;
	    free(fh) ;

	    // Return OK(0)/KO(-1)
	    return ret ;
	}

	int server_files_read ( long fd, void *buff_data, int count )
	{
	    int ret ;
	    MPI_File *fh ;
	    MPI_Status status ;

	    fh = (MPI_File *)fd ;
	    ret = MPI_File_read(*fh, buff_data, count, MPI_CHAR, &status) ;
	    if (ret != MPI_SUCCESS) {
		mfs_print(DBG_INFO, "ERROR: read %d bytes from file '%d'\n", count, fd) ;
		return -1 ;
	    }

	    // Return number_bytes
	    return count ;
	}

	int server_files_write ( long fd, void *buff_data, int count )
	{
	    int ret ;
	    MPI_File *fh ;
	    MPI_Status status ;

	    fh = (MPI_File *)fd ;
	    ret = MPI_File_write(*fh, buff_data, count, MPI_CHAR, &status) ;
	    if (ret != MPI_SUCCESS) {
		mfs_print(DBG_INFO, "ERROR: write %d bytes from file '%d'\n", count, fd) ;
		return -1 ;
	    }

	    // Return number_bytes
	    return count ;
	}

	void * server_files_mmap ( void *addr, size_t size, int protect, int flags, long filedes, off_t offset )
	{
	/*
	    void *ptr ;

	    ptr = mmap(addr, size, protect, flags, filedes, offset) ;
	    if (ptr == MAP_FAILED) {
		mfs_print(DBG_INFO, "ERROR: mapping failed\n") ;
		return NULL ;
	    }

	    return ptr ;
	*/
	    return NULL ;
	}

	int    server_files_munmap ( void *addr, size_t size )
	{
	/*
	    int err ;

	    err = munmap(addr, size);
	    if (err != 0) {
		mfs_print(DBG_INFO, "ERROR: UnMapping failed\n") ;
		return -1;
	    }
	*/

	    // Return OK
	    return 1 ;
	}

#endif

