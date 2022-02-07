
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
        case FILE_USE_MMAP:
             return (long) fd->mmap_ptr ;
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
        case FILE_USE_MMAP:
             fd->posix_fd = (int)0 ;
             fd->offset   = 0 ;         // TODO: how to recover the current working offset
             fd->size     = 1024*1024 ; // TODO: how to recover the current file size
             fd->file_protocol_name = "MMAP" ;
	     fd->mmap_ptr = (char *)fref ;
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
    printf("%s:   + mmap_ptr=%p\n", prefix, fd->mmap_ptr) ;
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
        case FILE_USE_MMAP:
	     // open
             fd->file_protocol_name = "MMAP" ;
	     fd->posix_fd = (long)open(path_name, flags, 0755) ;
	     if (fd->posix_fd < 0) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on open(name='%s', flags=%d, mode=0755)\n", path_name, flags) ;
	         return -1 ;
	     }
	     // prepare prot and flags
             int m_prot ;
             int m_flags ;
             m_flags = flags & O_ACCMODE ;
	     switch (m_flags)
	     {
	           case O_WRONLY:
		        m_prot  = PROT_WRITE | PROT_READ ;
		        m_flags = MAP_SHARED ;
			break ;
	           case O_RDONLY:
		        m_prot  = PROT_READ ;
		        m_flags = MAP_PRIVATE ;
			break ;
		   default:
		        m_prot  = PROT_WRITE | PROT_READ ;
		        m_flags = MAP_SHARED ;
			break ;
	     }
	     // prepare st_size
	     struct stat fdstat ;
	     fstat(fd->posix_fd, &fdstat) ;
             fd->size = fdstat.st_size ;
	     if (0 == fd->size) {
		 ftruncate(fd->posix_fd, 1024*1024) ;
	         fd->size = 1024*1024 ;
	     }
	     // mmap
	     fd->mmap_ptr = (char *)mmap(NULL, fd->size, m_prot, m_flags, fd->posix_fd, 0) ;
	     if (fd->mmap_ptr == MAP_FAILED)
	     {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on mapping failed\n") ;
                 perror("mmap: ") ;

                 close(fd->posix_fd) ;
                 fd->posix_fd = -1 ;
	         return -1 ;
	     }
	     // close
             ret = close(fd->posix_fd) ;
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
        case FILE_USE_MMAP:
	     // msync
	     msync(fd->mmap_ptr, fd->size, MS_SYNC);
	     // munmap
             ret = munmap(fd->mmap_ptr, fd->size);
             if (ret != 0) {
	         mfs_print(DBG_INFO, "[FILE]: ERROR on UnMapping failed\n") ;
             }
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
        case FILE_USE_MMAP:
             to_read = fd->size - fd->offset ;
             to_read = (count > to_read) ? to_read : count ;
             // memmove(buff_data, fd->mmap_ptr + fd->offset, to_read) ;  // TODO
             fd->offset = fd->offset + to_read ;

	     ret = to_read ;
             break ;
        default:
	     return -1 ;
             break ;
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
        case FILE_USE_MMAP:
             // memmove(fd->mmap_ptr + fd->offset, buff_data, count) ;  // TODO
             fd->offset = fd->offset + count ;

	     ret = count ;
             break ;
        default:
	     return -1 ;
             break ;
    }

    // Stats...
    fd->n_write_req ++ ;

    // Return number_bytes / KO
    return ret ;
}

