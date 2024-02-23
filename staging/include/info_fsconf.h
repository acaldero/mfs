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

#ifndef _MFS_CONF_H_
#define _MFS_CONF_H_

   #include "base_lib.h"
   #include "base_url.h"

   #include <yaml.h>


   // Datatype
   typedef struct
   {
	char        *name ;
	char        *type ;
	int          n_nodes ;
	char       **nodes ;
        base_url_t  *url ;
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

   // read, show and free
   int   info_fsconf_get  ( conf_t *conf, char *conf_file_name ) ;
   int   info_fsconf_show ( conf_t *conf ) ;
   int   info_fsconf_free ( conf_t *conf ) ;

   // getters
   int         info_fsconf_get_active_nnodes ( conf_t *conf ) ;
   char       *info_fsconf_get_active_node   ( conf_t *conf, int rank ) ;
   base_url_t *info_fsconf_get_active_url    ( conf_t *conf, int rank ) ;

   int         info_fsconf_get_partition_nnodes ( conf_part_t *partition ) ;
   char       *info_fsconf_get_partition_node   ( conf_part_t *partition, int rank ) ;
   base_url_t *info_fsconf_get_partition_url    ( conf_part_t *partition, int rank ) ;

#endif

