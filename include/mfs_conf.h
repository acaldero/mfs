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

#ifndef _MFS_CONF_H_
#define _MFS_CONF_H_

   #include <stdlib.h>
   #include <stdio.h>
   #include <string.h>
   #include <yaml.h>

   #include "mfs_lib.h"


   // Datatype
   typedef struct
   {
	 char   *name ;
	 char   *type ;
	 int     n_nodes ;
	 char  **nodes ;
   } conf_part_t ;

   typedef struct
   {
        int           n_partitions ;
        conf_part_t  *partitions ;
        conf_part_t  *active ;
   } conf_t ;


   /*
    * API
    */

   int   mfs_conf_get  ( conf_t *conf, char *conf_file_name ) ;
   int   mfs_conf_free ( conf_t *conf ) ;

   int   mfs_conf_show ( conf_t *conf ) ;
   char *mfs_conf_get_active_node ( conf_t *conf, int rank ) ;

#endif

