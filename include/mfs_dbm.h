
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

#ifndef __MFS_DBM_H__
#define __MFS_DBM_H__

    // Includes
    #include "base_lib.h"
    #include "base_string.h"


    // Datatypes
    class mfs_dbm
    {
      protected:
        int   been_used ;
        int   dbm_fd ;

        // underlying protocol
        int   dbm_backend ;
        char *dbm_backend_name ;

        // some stats
        long  n_read_req ;
        long  n_write_req ;

      public:
        virtual int   open       ( const char *path_name, int flags ) = 0 ;
        virtual int   close      ( void ) = 0 ;

        virtual int   store      ( void *buff_key, int count_key, void *buff_val, int  count_val ) = 0 ;
        virtual int   fetch      ( void *buff_key, int count_key, void *buff_val, int *count_val ) = 0 ;
        virtual int   del        ( void *buff_key, int count_key ) = 0 ;

        virtual int   stats_show ( char *prefix ) = 0 ;
    } ;


#endif

