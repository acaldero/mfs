
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

#ifndef __INFO_NS_MFS_H__
#define __INFO_NS_MFS_H__

    // Includes
    #include "base_lib.h"
    #include "base_string.h"
    #include "info_ns.h"

    // Class
    class info_ns_mfs : info_ns
    {
      protected:

      public:
	 info_ns_mfs ( ) ;
	~info_ns_mfs ( ) ;

        int  insert  ( char *srv_name, char *port_name ) ;
        int  lookup  ( char *srv_name, char *port_name, int *port_name_size ) ;
        int  remove  ( char *srv_name ) ;
    } ;

#endif
