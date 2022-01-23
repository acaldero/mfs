
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

int server_files_open ( const char *pathname, int flags )
{
    int fd ;

    // open file
    fd = open(pathname, flags, 0755) ;
    if (fd < 0) {
	mfs_print(DBG_INFO, "ERROR: on open(pathname='%s', flags=%d, mode=0755)\n", pathname, flags) ;
    }

    // Return file_descriptor
    return fd ;
}

int server_files_close ( int fd )
{
    // close file
    close(fd) ;

    // Return OK
    return 1 ;
}

int server_files_read ( int fd, void *buff_data, int count )
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

int server_files_write ( int fd, void *buff_data, int count )
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

void * server_files_mmap ( void *addr, size_t size, int protect, int flags, int filedes, off_t offset )
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

