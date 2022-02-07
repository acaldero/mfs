
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

    switch (fd->file_protocol)
    {
        case FILE_USE_POSIX:
             return (long) fd->posix_fd ;
             break ;
        case FILE_USE_MPI_IO:
             return (long) fd->mpiio_fd ;
             break ;
        default:
	     return -1 ;
             break ;
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

    // initialize to empty
    memset(fd, 0, sizeof(file_t)) ;

    // set values
    fd->file_protocol = file_protocol ;
    switch (fd->file_protocol)
    {
        case FILE_USE_POSIX:
             fd->posix_fd = (int)fref ;
             fd->offset   = lseek(fd->posix_fd, 0L, SEEK_CUR) ;
             fd->file_protocol_name = "POSIX" ;
             break ;
        case FILE_USE_MPI_IO:
             fd->mpiio_fd = (MPI_File)fref ;
             fd->offset   = 0 ;
             fd->file_protocol_name = "MPI-IO" ;
             break ;
        default:
             fd->file_protocol_name = "unknown" ;
	     return -1 ;
             break ;
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

    // Print stats...
    printf("%s: File:\n",           prefix) ;
    printf("%s: + protocol=%s\n",   prefix, fd->file_protocol_name) ;
    printf("%s:   + posix_fd=%d\n", prefix, fd->posix_fd) ;
    printf("%s:   + mpiio_fd=%d\n", prefix, fd->mpiio_fd) ;
    printf("%s: + offset=%ld\n",    prefix, fd->offset) ;
    printf("%s: + # read=%ld\n",    prefix, fd->n_read_req) ;
    printf("%s: + # write=%ld\n",   prefix, fd->n_write_req) ;

    // Return OK
    return 1 ;
}


/*
 *  File System API
 */

int  mfs_file_open ( file_t *fd, int file_protocol, const char *path_name, int flags )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Initialize fd
    memset(fd, 0, sizeof(file_t)) ;

    // Open file
    fd->file_protocol = file_protocol ;
    switch (fd->file_protocol)
    {
        case FILE_USE_POSIX:
             fd->file_protocol_name = "POSIX" ;
             fd->posix_fd = (long)open(path_name, flags, 0755) ;
             if (fd->posix_fd < 0) {
    	         mfs_print(DBG_INFO,
                           "[FILE]: ERROR on open(path_name='%s', flags=%d, mode=0755)\n", path_name, flags) ;
	         return -1 ;
             }
             break ;
        case FILE_USE_MPI_IO:
             fd->file_protocol_name = "MPI-IO" ;
	     ret = MPI_File_open(MPI_COMM_SELF, path_name,
                                 MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &(fd->mpiio_fd));
	     if (ret != MPI_SUCCESS) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on open('%s') file.\n", path_name) ;
	         return -1 ;
	     }
             break ;
        default:
	     return -1 ;
             break ;
    }

    // Return OK
    return 1 ;
}

int   mfs_file_close ( file_t *fd )
{
    int ret = -1 ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Close file
    switch (fd->file_protocol)
    {
        case FILE_USE_POSIX:
             ret = close(fd->posix_fd) ;
             break ;
        case FILE_USE_MPI_IO:
	     ret = MPI_File_close(&(fd->mpiio_fd)) ;
             break ;
        default:
	     return -1 ;
             break ;
    }

    // Return OK/KO
    return ret ;
}

int   mfs_file_read  ( file_t *fd, void *buff_data, int count )
{
    int ret = -1 ;
    long to_read ;

    // Check params...
    if (NULL == fd) {
	return -1 ;
    }

    // Read from file...
    switch (fd->file_protocol)
    {
        case FILE_USE_POSIX:
             ret = read(fd->posix_fd, buff_data, count) ;
             if (ret < 0) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", count, fd->posix_fd) ;
	         return -1 ;
             }
             break ;
        case FILE_USE_MPI_IO:
             MPI_Status status ;
	     ret = MPI_File_read(fd->mpiio_fd, buff_data, count, MPI_CHAR, &status) ;
	     if (ret != MPI_SUCCESS) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on read %d bytes from file '%d'\n", count, fd->mpiio_fd) ;
	         return -1 ;
	     }

	     MPI_Get_count(&status, MPI_INT, &ret);
             break ;
        default:
	     return -1 ;
             break ;
    }

    // Stats...
    (fd->n_read_req) ++ ;

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
    switch (fd->file_protocol)
    {
        case FILE_USE_POSIX:
             ret = write(fd->posix_fd, buff_data, count) ;
             if (ret < 0) {
    	         mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes from file '%d'\n", count, fd->posix_fd) ;
	         return -1 ;
             }
             break ;
        case FILE_USE_MPI_IO:
	     MPI_Status status ;
	     ret = MPI_File_write(fd->mpiio_fd, buff_data, count, MPI_CHAR, &status) ;
	     if (ret != MPI_SUCCESS) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on write %d bytes from file '%d'\n", count, fd->mpiio_fd) ;
	         return -1 ;
	     }

	     MPI_Get_count(&status, MPI_INT, &ret);
             break ;
        default:
	     return -1 ;
             break ;
    }

    // Stats...
    (fd->n_write_req) ++ ;

    // Return number_bytes / KO
    return ret ;
}

