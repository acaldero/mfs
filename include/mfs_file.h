
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

#ifndef __MFS_FILE_H__
#define __MFS_FILE_H__

    // Includes
    #include "base_lib.h"


    // Datatypes
    class mfs_file
    {
      protected:
        // underlying backend
        int   file_backend ;
        char *file_backend_name ;

        // some stats
        long  n_read_req ;
        long  n_write_req ;

      public:
        virtual int   open       ( const char *path_name, int flags, int mode ) = 0 ;
        virtual int   close      ( void ) = 0 ;
        virtual int   read       ( void *buff_data, int count ) = 0 ;
        virtual int   write      ( void *buff_data, int count ) = 0 ;

        virtual int   stats_show ( char *prefix ) = 0 ;

    } ;


#endif

