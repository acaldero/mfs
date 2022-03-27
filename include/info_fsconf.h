/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MFS.
 *
 *  MFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _MFS_CONF_H_
#define _MFS_CONF_H_

   #include "base_lib.h"

   #include <yaml.h>


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

   int   info_fsconf_get  ( conf_t *conf, char *conf_file_name ) ;
   int   info_fsconf_free ( conf_t *conf ) ;

   int   info_fsconf_show ( conf_t *conf ) ;
   char *info_fsconf_get_active_node ( conf_t *conf, int rank ) ;

#endif

