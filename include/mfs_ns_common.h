
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

#ifndef __MFS_NS_COMMON_H__
#define __MFS_NS_COMMON_H__

    // Includes
    #include <stdio.h>
    #include <stdlib.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>


    // File protocol
    #define NS_USE_MFS   1
    #define NS_USE_DBM   2
    #define NS_USE_FILE  3

    // Consts
    #define MAXPATHLEN    (1024)
    #define NS_FILE_NAME  "ns.data"


#endif

