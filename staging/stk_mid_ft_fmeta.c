
/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
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


   /* ... Include / Inclusion ........................................... */

      #include "include/stk_mid_ft_fmeta.h"


   /* ... Functions / Funciones ......................................... */

      int stk_mid_ft_fmeta_initByValues
      (
          fmeta_t  *fmeta,
          int       block_size,
          int       servers_count,
          int       start_server,
          char     *filesystem_tag,
          char     *partition_name,
          int       nerrors,
          int      *servers,
          int      *servers_status,
          int      *Ld,
          int      *Lr,
          int      *Ld2r,
          int      *Lr2d
      )
      {
          XPN_DEBUG_BEGIN_CUSTOM("fmeta:%p, block_size:%d, server_count:%d, start_server:%d, filesystem_tag:%s, nerrors:%d, servers:%p, servers_status:%p, Ld:%p, Lr:%p, Ld2r:%p, Lr2d:%p",
                                 fmeta,block_size,servers_count,start_server,
                                 filesystem_tag,
                                 nerrors,servers,servers_status,Ld,Lr,Ld2r,Lr2d);

	  /* check params */
	  if (NULL == fmeta)
	       return (-1) ;

	  /* set metadata information */
	  fmeta->block_size     = block_size ;
	  fmeta->servers_count  = servers_count ;
          fmeta->start_server   = start_server ;
	  fmeta->filesystem_tag = STRING_MISC_StrDup(filesystem_tag) ;
	  fmeta->partition_name = STRING_MISC_StrDup(partition_name) ;
          fmeta->nerrors        = nerrors ;
          fmeta->servers        = servers ;
          fmeta->servers_status = servers_status ;
          fmeta->Ld             = Ld ;
          fmeta->Lr             = Lr ;
          fmeta->Ld2r           = Ld2r ;
          fmeta->Lr2d           = Lr2d ;

          XPN_DEBUG_END_CUSTOM("fmeta:%p, block_size:%d, server_count:%d, start_server:%d, filesystem_tag:%s, nerrors:%d, servers:%p, servers_status:%p, Ld:%p, Lr:%p, Ld2r:%p, Lr2d:%p",
                               fmeta,block_size,servers_count,start_server,
                               filesystem_tag,
                               nerrors,servers,servers_status,Ld,Lr,Ld2r,Lr2d);

          /* return ok */
	  return 1 ;
      }

      int stk_mid_ft_fmeta_finalize ( fmeta_t *fmeta )
      {
          XPN_DEBUG_BEGIN_CUSTOM("fmeta:%p", fmeta) ;

	  /* check params */
	  if (NULL == fmeta)
	       return (-1) ;

	  /* free resources */
	  if (NULL != fmeta->filesystem_tag)
	  {
	  	free(fmeta->filesystem_tag) ;
		fmeta->filesystem_tag = NULL ;
	  }

	  if (NULL != fmeta->partition_name)
	  {
		free(fmeta->partition_name) ;
		fmeta->partition_name = NULL ;
	  }

          XPN_DEBUG_END_CUSTOM("fmeta:%p", fmeta) ;

          /* return ok */
	  return (1);
      }


   /* ................................................................... */

