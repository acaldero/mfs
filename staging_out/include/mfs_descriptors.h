
/*
 *  Copyright 2020-2024 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of XPNlite.
 *
 *  XPNlite is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPNlite is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with XPNlite.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MFS_DESCRIPTORS_H__
#define __MFS_DESCRIPTORS_H__

    // Includes
    #include "base_lib.h"


    // Datatypes
    typedef struct {
         pthread_mutex_t   mutex ;
         int               hash_neltos ;
         void             *hash_eltos ;
         int              *been_used ;
         int              *file_fd ;
    } descriptor_t ;


    // API
    int   mfs_descriptor_init      ( descriptor_t *des, int neltos, int elto_size ) ;
    int   mfs_descriptor_finalize  ( descriptor_t *des ) ;

    int   mfs_descriptor_find_free ( descriptor_t *des, int elto_size ) ;
    void  mfs_descriptor_set_free  ( descriptor_t *des, int fd ) ;
    int   mfs_descriptor_is_free   ( descriptor_t *des, int fd ) ;

    void *mfs_descriptor_fd2fh     ( descriptor_t *des, int fd, int elto_size ) ;

#endif

