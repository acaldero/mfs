
/*
 *  Copyright 2020-2021 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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

#include "mfs_lib.h"


//
// Print for debug
//

int mfs_print ( FILE *fd, char *fmt_str, ... )
{
   va_list valist ;
   int ret ;

   va_start(valist, fmt_str) ;
#ifdef DEBUG
   ret = vfprintf(fd, fmt_str, valist) ;
#endif
   va_end(valist);

   return ret ;
}


//
// Server port
//

int mfs_write_server_port ( char *port_name, int rank )
{
    char file_name[1024] ;

    // File name
    sprintf(file_name, "mfs_server.port.%d", rank) ;

    // Write server port...
    FILE *fd = fopen(file_name, "w") ;
    if (fd == NULL) {
        fprintf(stderr, "ERROR: fopen fails for %s :-S", file_name) ;
        return -1 ;
    }
    fprintf(fd, "%s\n", port_name) ;
    fclose(fd) ;

    // Return OK
    return 0 ;
}

int mfs_read_server_port ( char *port_name, int rank )
{
    char file_name[1024] ;

    // File name
    sprintf(file_name, "mfs_server.port.%d", rank) ;

    // Read server port...
    FILE *fd = fopen(file_name, "r") ;
    if (fd == NULL) {
        fprintf(stderr, "ERROR: fopen fails for %s :-S", file_name) ;
        return -1 ;
    }
    fgets(port_name, MPI_MAX_PORT_NAME, fd) ;
    fclose(fd);

    // Return OK
    return 0 ;
}

