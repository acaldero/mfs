
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

#ifndef __MFS_NS_COMMON_H__
#define __MFS_NS_COMMON_H__

    // Includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>


    // File protocol
    #define NS_USE_DBM   1
    #define NS_USE_FILE  2
    #define NS_USE_MFS   3
    #define NS_USE_TDB   4

    // Consts
    #define MAXPATHLEN      (1024)
    #define NS_FILE_NAME    "ns.data"
    #define NS_DEFAULT_PORT 12345

#endif

