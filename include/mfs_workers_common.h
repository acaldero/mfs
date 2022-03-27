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

#ifndef __MFS_WORKERS_COMMON_H__
#define __MFS_WORKERS_COMMON_H__

   #include "base_lib.h"
   #include "info_params.h"
   #include "stub_msg.h"

   #include <pthread.h>


   /*
    * Constants
    */

   #define MAX_THREADS 1024
   #define STACK_SIZE (256*1024)


   /*
    * Datatype
    */

   typedef struct st_th th_args_t ;

   struct st_th
   {
        comm_t    ab ;
        void (*function)(struct st_th) ;
   };

#endif

