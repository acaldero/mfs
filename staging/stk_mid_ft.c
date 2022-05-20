
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

      #include "include/stk_mid_ft.h"


   /* ... Internal functions / Funciones internas ....................... */

      char *stk_mid_ft_metaData_getMetaDataFileName ( int fd )
      {
        char  *pfname ;
        int    pfname_len ;

	XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

        pfname_len = strlen(xpni_fit_get_XPN_FNAME(fd)) +
                     strlen(".stk_mid_ft") +
                     1 /* '\0'*/ ;

        pfname = (char *)malloc(pfname_len) ;
        if (NULL == pfname)
 	    return NULL ;

	strcpy(pfname,xpni_fit_get_XPN_FNAME(fd)) ;
	strcat(pfname,".stk_mid_ft") ;

	XPN_DEBUG_END_CUSTOM("%d", fd) ;

        return pfname ;
      }

      ssize_t stk_mid_ft_sread_fail_r5o
      (
         int fd,
         void *buffer,
         off_t offset,
         size_t size
      )
      {

        int ret ;
        int block_index1;      /* Index of associated first data block */
        int block_offset1;     /* Offset inside associated to first data block */
        int block_index2;      /* Index of associated last data block */
        int block_offset2;     /* Offset inside associated to last data block */
        int SPi;               /* Server for Parity */
        int IPi;               /* Index for Parity */
        int SDi;               /* Server for Data */
        int IDi;               /* Index for Data */
        fmeta_t fmeta;         /* File metadata (extended version) */
	char *buffer_row;
	int toRecover, i, j;

        /* debugging */
	XPN_DEBUG_BEGIN_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;

        fmeta = xpni_fit_get_XPN_FMETA(fd) ;

        block_index1   = (offset)      / fmeta.block_size ;
        block_offset1  = (offset)      % fmeta.block_size ;
        block_index2   = (offset+size) / fmeta.block_size ;
        block_offset2  = (offset+size) % fmeta.block_size ;

	toRecover = 0 ;
	for (i=block_index1; i<block_index2; i++)
	{
           ret = MATH_MISC_locateInRAID5withInternalParity(i,
                                                           fmeta.servers_count,
                                                           &SPi,&IPi,&SDi,&IDi);
	   if (1 == SDi) {
	       toRecover++ ;
	   }
	}

	if (0 == toRecover)
	{
		ret = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd),
					 buffer,
					 (int)offset,
					 size) ;
	}
	else
	{
		buffer_row = (char *)malloc(fmeta.block_size*fmeta.servers_count);
		if (NULL == buffer_row)
		            return (-1) ;
		memset(buffer_row,0,fmeta.block_size*fmeta.servers_count);

	        for (i=0; i<toRecover; i++)
		{
	                for (j=0; j<fmeta.servers_count-1; j++)
			{
				ret = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd),
							 buffer_row,
							 (int)0,
							 fmeta.block_size) ;
			}

			ret = MATH_MISC_Xor(buffer_row,
			                    buffer_row,
				            buffer_row,
				            fmeta.block_size);

		        ret = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd),
					         buffer,
					         (int)offset,
					         size) ;
		}

		free(buffer_row);
	}

        /* debugging */
	XPN_DEBUG_END_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;

	return ret ;
      }

      ssize_t stk_mid_ft_sread_nofail_r5o
      (
         int fd,
         void *buffer,
         off_t offset,
         size_t size
      )
      {
         ssize_t ret ;

	 XPN_DEBUG_BEGIN_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;
	 ret = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd), buffer, (int)offset, size) ;
	 XPN_DEBUG_END_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;

	 return ret ;
      }

      ssize_t stk_mid_ft_sread_r5o
      (
        int fd,
        void *buffer,
        off_t offset,
        size_t size
      )
      {
         char *fmeta_fsTag;     /* File metadata (file system tag) */
         int   fmeta_nerrors;   /* File metadata (number of errors) */
         ssize_t   ret;         /* Returned value from called functions */

         /* debugging */
	 XPN_DEBUG_BEGIN_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;

         /* check params */
         if ( ! xpni_fit_is_correct(fd) )
             return (-1) ;

         /* Select (file system type) x (status) */
         fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
         fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

         ret = -1 ;
         if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
	 {
		switch (fmeta_nerrors)
		{
			case 0:
                             ret = stk_mid_ft_sread_nofail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			case 1:
                             ret = stk_mid_ft_sread_fail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			default:
                             ret = (-1);
			     break;
		}
	 }

         /* debugging */
	 XPN_DEBUG_END_CUSTOM("%d,%p,%d,%d -> %d", fd,buffer,(int)offset,size, ret) ;

         /* Return bytes written */
         return ret ;
      }


      ssize_t stk_mid_ft_swrite_nofail_r5o
      (
        int fd,
        void *buffer,
        off_t offset,
        size_t size
      )
      {
        ssize_t ret;           /* Returned value from called functions */
        int block_index1;      /* Index of associated first data block */
        int block_offset1;     /* Offset inside associated to first data block */
        int block_index2;      /* Index of associated last data block */
        int block_offset2;     /* Offset inside associated to last data block */
        int SA;                /* Server will be read/write */
        int SPi;               /* Server for Parity */
        int IPi;               /* Index for Parity */
        int SDi;               /* Server for Data */
        int IDi;               /* Index for Data */
        int parity_indexi;     /* Index of associate parity block */
        int parity_offseti;    /* Offset inside associate parity block */
        int data_indexi;       /* Index of associate data block */
        int data_offseti;      /* Offset inside associate data block */
        int end_indexi;        /* Index of associate end block */
        int end_offseti;       /* Offset inside associate end block */
        int begin_indexi;      /* Index of associate begin data block */
        int begin_offseti;     /* Offset inside associate begin data block */
        char *buffer_i;        /* Auxiliar pointer */
        char *buffer_parity;   /* Buffer for parity block */
        char *buffer_data;     /* Buffer for data block */
        char *block_bufferi;   /* Aditional auxiliar pointer */
        char *buffer_row;      /* Buffer for a row */
        fmeta_t fmeta;         /* File metadata (extended version) */
        int rows;              /* Rows between block_index1 and block_index2 */
        int row;               /* Auxiliar counter for rows */
        int block_i;           /* Auxiliar counter for blocks */
        int offseti;           /* Auxiliar counter for data block offset */
        int block_offseti;     /* Auxiliar counter for data block offset */
	int       aux2;        /* Auxiliar for some intermediate computations */
	int to_write;          /* Auxiliar for how many bytes write in a block */
	int to_write2;         /* Auxiliar for how many bytes write in total */


         /* debugging */
	 XPN_DEBUG_BEGIN_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;


         /*
          * Locate logical block position (first and last blocks)
          */

         fmeta = xpni_fit_get_XPN_FMETA(fd) ;

         block_index1   = (offset)      / fmeta.block_size ;
         block_offset1  = (offset)      % fmeta.block_size ;
         block_index2   = (offset+size) / fmeta.block_size ;
         block_offset2  = (offset+size) % fmeta.block_size ;


         /*
          * Alloc memory
          */

         buffer_i   = (char *)buffer;
         buffer_row = (char *)malloc(fmeta.block_size*fmeta.servers_count);
         if (NULL == buffer_row)
             return (-1) ;


        /*
         * Read physical block positions
         */

	/* Physical rows to read... */
	aux2 = (fmeta.servers_count - 1) ;
	rows = (block_index2 / aux2) - (block_index1 / aux2) + 1;

        block_i       = block_index1;
	block_offseti = block_offset1;

        for (row=0; row<rows; row++)
        {
            ret = MATH_MISC_locateInRAID5withExternalParity(block_i,
                                                            fmeta.servers_count,
                                                            &SPi,&IPi,&SDi,&IDi);

            parity_indexi  = IPi * fmeta.servers_count + SPi ;
            data_indexi    = block_i ;
	    begin_indexi   = data_indexi - (data_indexi % aux2) ; /* (data_indexi / aux2) * aux2 ; */

	    /* end_indexi is not included */
	    if ((rows - 1) == row) /* última fila*/
	         end_indexi = block_index2 + (block_offset2 != 0) ;
	    else
	         end_indexi = begin_indexi + aux2 ;

            begin_offseti  = begin_indexi  * fmeta.block_size;
            end_offseti    = end_indexi    * fmeta.block_size;
            parity_offseti = parity_indexi * fmeta.block_size;
            data_offseti   = data_indexi   * fmeta.block_size;

            /* read full data row */
            ret = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd),
                                    buffer_row,
                                    begin_offseti,
                                    fmeta.block_size*aux2);

            /* read old parity for this row */
            buffer_parity = buffer_row + fmeta.block_size*aux2;
            ret = xpni_lowfsi_pread(xpni_fit_get_XPN_MDATA_FD(fd),
                                    buffer_parity,
                                    parity_offseti,
                                    fmeta.block_size);

            /* for each data block: write new parity + write data */
            for
            (
                 offseti  = data_offseti;
                 offseti != end_offseti;
                 offseti += fmeta.block_size
            )
            {
               /* get old data */
               buffer_data = buffer_row + (offseti - begin_offseti) ;

               /* compute new parity */
               ret = MATH_MISC_Xor(buffer_parity,
                                   buffer_parity,
                                   buffer_data,
                                   fmeta.block_size);

	       /* by default: write data from buffer_i (piece from buffer param)... */
               block_bufferi = buffer_i;

	       /* compute how many bytes will be written. */
	       to_write  = fmeta.block_size - block_offseti; /* bytes left to be written in a block */

	       if (offseti > offset)
	           to_write2 = size - (offseti - offset); /* total bytes left to be written */
	       else
		   to_write2 = size ; /* (offseti =< offset): first block, no data was written */

	       if (to_write > to_write2)
		   to_write = to_write2;

               /* ... but handle first data block (not all in buffer_i)... */
               if (0 != block_offseti)
               {
                     block_bufferi = buffer_data;

                     memmove(buffer_data+block_offseti,
                             buffer,
                             to_write);
		     to_write = to_write + block_offseti ;
               }

	       /* compute the new parity block */
               ret = MATH_MISC_Xor(buffer_parity,
                                   buffer_parity,
                                   block_bufferi,
                                   to_write);

               /* write the new parity block */
               SA = SPi;
               ret = xpni_lowfsi_pwrite(xpni_fit_get_XPN_MDATA_FD(fd),
                                        buffer_parity,
                                        parity_offseti,
                                        fmeta.block_size);

#if defined(XPN2_DEBUG)
	       if (ret < fmeta.block_size)
	           DEBUG_MSG_PrintF(ERROR,"xpni_lowfsi_pwrite -> %d.\n",ret);
#endif


               /* write data */
               SA = SDi;
               ret = xpni_lowfsi_pwrite(xpni_fit_get_XPN_DATA_FD(fd),
                                        block_bufferi,
                                        offseti,
                                        to_write);

#if defined(XPN2_DEBUG)
	       if (ret < fmeta.block_size)
	           DEBUG_MSG_PrintF(ERROR,"xpni_lowfsi_pwrite -> %d.\n",ret);
#endif

               buffer_i = buffer_i + fmeta.block_size - block_offseti;
	       block_offseti = 0;
            }

	    /* update the block_i: the first block of next row */
	    if (0 == row)
		block_i = block_i - (block_i % (fmeta.servers_count - 1)) ;

            block_i = block_i + (fmeta.servers_count - 1) ;
         }


         /*
          * Free memory
          */
         free(buffer_row) ;

         /* debugging */
	 XPN_DEBUG_END_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;

         /*
          * Return bytes readed
          */
         return size ;
      }

      ssize_t stk_mid_ft_swrite_fail_r5o
      (
        int fd,
        void *buffer,
        off_t offset,
        size_t size
      )
      {
        ssize_t ret;           /* Returned value from called functions */
        int block_index1;      /* Index of associated first data block */
        int block_offset1;     /* Offset inside associated to first data block */
        int block_index2;      /* Index of associated last data block */
        int block_offset2;     /* Offset inside associated to last data block */
        int SA;                /* Server will be read/write */
        int SPi;               /* Server for Parity */
        int IPi;               /* Index for Parity */
        int SDi;               /* Server for Data */
        int IDi;               /* Index for Data */
        int parity_indexi;     /* Index of associate parity block */
        int parity_offseti;    /* Offset inside associate parity block */
        int data_indexi;       /* Index of associate data block */
        int data_offseti;      /* Offset inside associate data block */
        int end_indexi;        /* Index of associate end block */
        int end_offseti;       /* Offset inside associate end block */
        int begin_indexi;      /* Index of associate begin data block */
        int begin_offseti;     /* Offset inside associate begin data block */
        char *buffer_i;        /* Auxiliar pointer */
        char *buffer_parity;   /* Buffer for parity block */
        char *buffer_data;     /* Buffer for data block */
        char *block_bufferi;   /* Aditional auxiliar pointer */
        char *buffer_row;      /* Buffer for a row */
        fmeta_t fmeta;         /* File metadata (extended version) */
        int rows;              /* Rows between block_index1 and block_index2 */
        int row;               /* Auxiliar counter for rows */
        int block_i;           /* Auxiliar counter for blocks */
        int offseti;           /* Auxiliar counter for data block offset */
        int block_offseti;     /* Auxiliar counter for data block offset */
	int       aux2;        /* Auxiliar for some intermediate computations */
	int to_write;          /* Auxiliar for how many bytes write in a block */
	int to_write2;         /* Auxiliar for how many bytes write in total */


         /* debugging */
	 XPN_DEBUG_BEGIN_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;


         /*
          * Locate logical block position (first and last blocks)
          */

         fmeta = xpni_fit_get_XPN_FMETA(fd) ;

         block_index1   = (offset)      / fmeta.block_size ;
         block_offset1  = (offset)      % fmeta.block_size ;
         block_index2   = (offset+size) / fmeta.block_size ;
         block_offset2  = (offset+size) % fmeta.block_size ;


         /*
          * Alloc memory
          */

         buffer_i   = (char *)buffer;
         buffer_row = (char *)malloc(fmeta.block_size*fmeta.servers_count);
         if (NULL == buffer_row)
             return (-1) ;


        /*
         * Read physical block positions
         */

	 /* Physical rows to read... */
	 aux2 = (fmeta.servers_count - 1) ;
	 rows = (block_index2 / aux2) - (block_index1 / aux2) + 1;

         block_i       = block_index1;
	 block_offseti = block_offset1;

         for (row=0; row<rows; row++)
         {
            ret = MATH_MISC_locateInRAID5withExternalParity(block_i,
                                                            fmeta.servers_count,
                                                            &SPi,&IPi,&SDi,&IDi);

            parity_indexi  = IPi * fmeta.servers_count + SPi ;
            data_indexi    = block_i ;
	    begin_indexi   = data_indexi - (data_indexi % aux2) ; /* (data_indexi / aux2) * aux2 ; */

	    /* end_indexi is not included */
	    if ((rows - 1) == row) /* última fila*/
	         end_indexi = block_index2 + (block_offset2 != 0) ;
	    else
	         end_indexi = begin_indexi + aux2 ;

            begin_offseti  = begin_indexi  * fmeta.block_size;
            end_offseti    = end_indexi    * fmeta.block_size;
            parity_offseti = parity_indexi * fmeta.block_size;
            data_offseti   = data_indexi   * fmeta.block_size;

            /* read full data row */
/* <Server 1 will fail ...> */
            ret = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd),
                                    buffer_row,
                                    begin_offseti,
                                    fmeta.block_size*aux2);

            /* read old parity for this row */
/* <Server 1 will fail ...> */
            buffer_parity = buffer_row + fmeta.block_size*aux2;
            ret = xpni_lowfsi_pread(xpni_fit_get_XPN_MDATA_FD(fd),
                                    buffer_parity,
                                    parity_offseti,
                                    fmeta.block_size);

            /* for each data block: write new parity + write data */
            for
            (
                 offseti  = data_offseti;
                 offseti != end_offseti;
                 offseti += fmeta.block_size
            )
            {
               /* get old data */
               buffer_data = buffer_row + (offseti - begin_offseti) ;

               /* compute new parity */
               ret = MATH_MISC_Xor(buffer_parity,
                                   buffer_parity,
                                   buffer_data,
                                   fmeta.block_size);

	       /* by default: write data from buffer_i (piece from buffer param)... */
               block_bufferi = buffer_i;

	       /* compute how many bytes will be written. */
	       to_write  = fmeta.block_size - block_offseti; /* bytes left to be written in a block */

	       if (offseti > offset)
	           to_write2 = size - (offseti - offset); /* total bytes left to be written */
	       else
		   to_write2 = size ; /* (offseti =< offset): first block, no data was written */

	       if (to_write > to_write2)
		   to_write = to_write2;

               /* ... but handle first data block (not all in buffer_i)... */
               if (0 != block_offseti)
               {
                     block_bufferi = buffer_data;

                     memmove(buffer_data+block_offseti,
                             buffer,
                             to_write);
		     to_write = to_write + block_offseti ;
               }

	       /* compute the new parity block */
               ret = MATH_MISC_Xor(buffer_parity,
                                   buffer_parity,
                                   block_bufferi,
                                   to_write);


               /* write the new parity block */
               SA = SPi;
               if (SPi != 1) /* if (SPi != <servidor_falla>) */
               {
                  ret = xpni_lowfsi_pwrite(xpni_fit_get_XPN_MDATA_FD(fd),
                                           buffer_parity,
                                           parity_offseti,
                                           fmeta.block_size);

#if defined(XPN2_DEBUG)
	          if (ret < fmeta.block_size)
	              DEBUG_MSG_PrintF(ERROR,"xpni_lowfsi_pwrite -> %d.\n",ret);
#endif
               }

               /* write data */
               SA = SDi;
               if (SDi != 1) /* if (SDi != <servidor_falla>) */
               {
/* <rebuild data block> */
                  ret = MATH_MISC_Xor(buffer_parity,
                                      buffer_parity,
                                      buffer_parity,
                                      fmeta.block_size);
/* </rebuild data block> */

                  ret = xpni_lowfsi_pwrite(xpni_fit_get_XPN_DATA_FD(fd),
                                           block_bufferi,
                                           offseti,
                                           to_write);

#if defined(XPN2_DEBUG)
   	          if (ret < fmeta.block_size)
	              DEBUG_MSG_PrintF(ERROR,"xpni_lowfsi_pwrite -> %d.\n",ret);
#endif
               }

               /* next buffer block, and next data server */
               buffer_i = buffer_i + fmeta.block_size - block_offseti;
	       block_offseti = 0;

               SDi = (SDi + 1) % aux2 ;
            }

	    /* update the block_i: the first block of next row */
	    if (0 == row)
		block_i = block_i - (block_i % (fmeta.servers_count - 1)) ;

            block_i = block_i + (fmeta.servers_count - 1) ;
         }


         /*
          * Free memory
          */
         free(buffer_row) ;

	 /* debugging */
	 XPN_DEBUG_END_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;

         /*
          * Return bytes readed
          */
         return size ;
      }

      ssize_t stk_mid_ft_swrite_r5o
      (
        int fd,
        void *buffer,
        off_t offset,
        size_t size
      )
      {
         char *fmeta_fsTag;     /* File metadata (file system tag) */
         int   fmeta_nerrors;   /* File metadata (number of errors) */
         ssize_t   ret;         /* Returned value from called functions */

         /* debugging */
	 XPN_DEBUG_BEGIN_CUSTOM("%d,%p,%d,%d", fd,buffer,(int)offset,size) ;

         /* check params */
         if ( ! xpni_fit_is_correct(fd) )
             return (-1) ;

         /* Select (file system type) x (status) */
         fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
         fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

         ret = -1 ;
         if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
	 {
		switch (fmeta_nerrors)
		{
			case 0:
                             ret = stk_mid_ft_swrite_nofail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			case 1:
                             ret = stk_mid_ft_swrite_fail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			default:
                             ret = -1 ;
			     break;
		}
	 }

         /* debugging */
	 XPN_DEBUG_END_CUSTOM("%d,%p,%d,%d -> %d", fd,buffer,(int)offset,size,ret) ;

         /* Return bytes written */
         return ret ;
      }

      int stk_mid_ft_exportFile_r5o ( int fd )
      {
	 char  *pfname ;
	 int    ret ;

         /* debugging */
	 XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

         /* parity file name */
         pfname = stk_mid_ft_metaData_getMetaDataFileName(fd) ;
         if (NULL == pfname)
	     return (-1) ;

         /* unlink file */
	 ret = xpn_unlink(pfname) ;

         /* free parity file name */
         free(pfname) ;

         /* debugging */
	 XPN_DEBUG_END_CUSTOM("%d", fd) ;

	 /* return 'ok' */
	 return (ret) ;
      }

      int stk_mid_ft_importFile_r5o ( int fd )
      {
	 int       ret;
	 int       i;
	 char     *pfname ;
	 fmeta_t   fmeta;
	 int       readed ;
	 int       len_d, len_p, len_a ;
	 char     *buf_d, *buf_p, **buf_a ;
	 int       offset_p, block_p, row_p, col_p ;
	 int       offset_d, block_d, row_d ;
	 int       block_per_row ;

         /* debugging */
	 XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

         /* parity file name */
	 pfname = stk_mid_ft_metaData_getMetaDataFileName(fd) ;
         if (NULL == pfname)
 	     return (-1) ;

         /* open parity file */
         ret = STK_FS_OPEN(pfname, O_CREAT|O_RDWR, S_IRWXU);
         if (ret < 0)
	     return (-1) ;

         /* update file description */
         xpni_fit_set_XPN_MDATA_FD(fd,ret) ;

         /* free resources */
	 free(pfname) ;

	 /* get file metadata */
	 fmeta = xpni_fit_get_XPN_FMETA(fd) ;

	 /* alloc buffers */
	 block_per_row = fmeta.servers_count - 1 ;

	 len_p = fmeta.block_size ;
	 buf_p = (char *)malloc(len_p) ;
	 if (NULL == buf_p)
		return (-1) ;

	 len_d = block_per_row * fmeta.block_size ;
	 buf_d = (char *)malloc(len_d) ;
	 if (NULL == buf_d)
	 {
		free(buf_p) ;
		return (-1) ;
	 }

	 len_a = block_per_row ;
	 buf_a = (char **)malloc(len_a*sizeof(char *)) ;
	 if (NULL == buf_a)
	 {
		free(buf_p) ;
		free(buf_d) ;
		return (-1) ;
	 }

	 /* read data and write associated parity */
	 readed   = len_d ;
	 offset_d = 0 ;
	 while (readed == len_d)
	 {
	   block_d  = (offset_d / fmeta.block_size) + ( (offset_d % fmeta.block_size) != 0 ) ;
	   row_d    = (block_d  / block_per_row)    + ( (block_d  % block_per_row) != 0 ) ;
	   row_p    = row_d / fmeta.servers_count ;
	   col_p    = block_per_row - (row_d % fmeta.servers_count) ;
	   block_p  = (fmeta.servers_count*row_p + col_p) ;
	   offset_p = (fmeta.servers_count*row_p + col_p) * fmeta.block_size ;

	   /* readed = read 'block_per_row' data blocks */
           readed = xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd),
		 		      buf_d,
				      offset_d,
				      len_d);

	   /* compute parity block */
	   for (i=0; i<block_per_row; i++)
		   buf_a[i] = buf_d+i*fmeta.block_size ;

           ret = MATH_MISC_XorN(buf_p,
                                buf_a,
                                len_a,
                                fmeta.block_size);

	   /* write '1' parity block */
           ret = xpni_lowfsi_pwrite(xpni_fit_get_XPN_MDATA_FD(fd),
                                    buf_p,
                                    offset_p,
                                    fmeta.block_size);

	   /* go to next data 'row' */
	   offset_d = offset_d + len_d ;
	 }

         /* debugging */
	 XPN_DEBUG_END_CUSTOM("%d", fd) ;

	 /* return ok */
	 return (1) ;
      }


   /* ... Functions / Funciones ......................................... */

      //
      // Init + finalize
      //

      int stk_mid_ft_init ( stk_fs_t *fsi, stk_fs_t *low_fsi )
      {
	  int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p, low_fsi:%p", fsi, low_fsi) ;

	  /* check params */
	  if (NULL == fsi) {
	      return -1 ;
	  }

	  /* register stk_mid_ft interface */
	  fsi->fsi_name       = STRING_MISC_StrDup("stk_mid_ft") ;
	  fsi->low_fsi        = low_fsi ;

	  // init/finalize
	  fsi->fsi_init       = stk_mid_ft_init ;
          fsi->fsi_finalize   = stk_mid_ft_finalize ;
	  // file API
          fsi->fsi_open       = stk_mid_ft_open ;
          fsi->fsi_creat      = stk_mid_ft_creat ;
          fsi->fsi_close      = stk_mid_ft_close ;
          fsi->fsi_read       = stk_mid_ft_read ;
          fsi->fsi_write      = stk_mid_ft_write ;
          fsi->fsi_lseek      = stk_mid_ft_lseek ;
	  // directory API
          fsi->fsi_opendir    = stk_mid_ft_opendir ;
          fsi->fsi_closedir   = stk_mid_ft_closedir ;
          fsi->fsi_readdir    = stk_mid_ft_readdir ;
          fsi->fsi_mkdir      = stk_mid_ft_mkdir ;
          fsi->fsi_rmdir      = stk_mid_ft_rmdir ;
          fsi->fsi_rewinddir  = stk_mid_ft_rewinddir ;

	  // TODO: check if stack builder should do this instead
	  ret = STK_FS_INIT(fsi->low_fsi, NULL) ;

	  XPN_DEBUG_END_CUSTOM("fsi:%p, low_fsi:%p -> %d", fsi, low_fsi, ret) ;

	  /* return ok */
	  return (1) ;
      }

      int stk_mid_ft_finalize ( stk_fs_t *fsi )
      {
	  int       ret ;
          stk_fs_t *low_fsi ;

	  XPN_DEBUG_BEGIN_CUSTOM("fsi:%p", fsi) ;

	  // TODO: check if stack builder should do this instead
          low_fsi = STK_FS_GET_LOWFSI(fsi) ;
	  if (NULL != low_fsi) {
              ret = STK_FS_FINALIZE(low_fsi) ;
	  }

	  // finalize
          ret = -1 ;
	  if (NULL != fsi) {
              ret = stk_fs_finalize(fsi) ;
	  }

	  XPN_DEBUG_END_CUSTOM("%p", fsi) ;

	  /* return ok|ko */
	  return ret ;
      }


      //
      // open + close + sread + swrite
      //

      int stk_mid_ft_creat ( stk_fs_t *fsi, char *path, mode_t mode )
      {
          char   *pfname ;
          int     ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("path:%s, mode:%d", path, (int)mode) ;

          /* check params */
          if (NULL == path) {
               return -1 ;
	  }

          /* parity file name */
	  pfname = (xpni_fit_get_XPN_FMETA(fd)).filesystem_tag ;
          if ( ! strcmp(pfname, FS_TAG_RAID5OUTER) )
          {
	       /* parity file name */
               pfname = stk_mid_ft_metaData_getMetaDataFileName(fd) ;
	       if (NULL == pfname)
		   return (-1) ;

	       /* open parity file */
	       ret = STK_FS_OPEN(pfname, O_CREAT|O_RDWR, S_IRWXU);
	       if (ret < 0)
		   return (-1) ;

	       /* update file description */
	       xpni_fit_set_XPN_MDATA_FD(fd, ret) ;

	       /* free resources */
 	       free(pfname) ;
          }

          /* open data file */
          ret = xpni_lowfsi_creat(xpni_fit_get_XPN_FNAME(fd), xpni_fit_get_XPN_MODE(fd)) ;
          if (ret < 0)
          {
              xpni_lowfsi_close(xpni_fit_get_XPN_MDATA_FD(fd)) ;
              return (-1) ;
          }

          /* update file description */
          xpni_fit_set_XPN_DATA_FD(fd, ret) ;

	  XPN_DEBUG_END_CUSTOM("path:%s, mode:%d -> %d", path, (int)mode, ret) ;

          /* return xpn file descriptor */
          return ret ;
      }

      int stk_mid_ft_open ( stk_fs_t *fsi, int fd )
      {
          char      *pfname ;
          int        ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("path:%s, flags:%d, mode:%d", path, flags, (int)mode) ;

          /* check params */
          if ( ! xpni_fit_is_correct(fd) ) {
               return (-1) ;
	  }

          /* parity file name */
          if ( ! strcmp
                (
                  (xpni_fit_get_XPN_FMETA(fd)).filesystem_tag,FS_TAG_RAID5OUTER
                )
          )
          {
	       /* parity file name */
               pfname = stk_mid_ft_metaData_getMetaDataFileName(fd) ;
	       if (NULL == pfname)
		   return (-1) ;

	       /* open parity file */
	       ret = STK_FS_OPEN(pfname,
                                      O_CREAT|O_RDWR,
                                      S_IRWXU);
	       if (ret < 0)
		   return (-1) ;

	       /* update file description */
	       xpni_fit_set_XPN_MDATA_FD(fd,ret) ;

	       /* free resources */
	       free(pfname) ;
          }

          /* open data file */
          ret = STK_FS_OPEN(xpni_fit_get_XPN_FNAME(fd),
			         xpni_fit_get_XPN_FLAG(fd),
			         xpni_fit_get_XPN_MODE(fd)) ;
          if (ret < 0)
          {
             xpni_lowfsi_close(xpni_fit_get_XPN_MDATA_FD(fd)) ;
             return (-1) ;
          }

          /* update file description */
          xpni_fit_set_XPN_DATA_FD(fd,ret) ;

	  XPN_DEBUG_END_CUSTOM("path:%s, flags:%d, mode:%d", path, flags, (int)mode) ;

          /* return xpn file descriptor */
          return ret ;
      }

      int stk_mid_ft_close ( stk_fs_t *fsi, int fd )
      {
	  int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

          /* check params */
          if ( ! xpni_fit_is_correct(fd) )
              return (-1) ;

          /* close file: data and metadata */
          if ( ! strcmp((xpni_fit_get_XPN_FMETA(fd)).filesystem_tag, FS_TAG_RAID5OUTER) )
          {
	      xpni_lowfsi_close(xpni_fit_get_XPN_MDATA_FD(fd));
          }

	  ret = xpni_lowfsi_close(xpni_fit_get_XPN_DATA_FD(fd));

	  XPN_DEBUG_END_CUSTOM("%d", fd) ;

          /* return ok|ko */
          return ret ;
      }

      off_t   stk_mid_ft_lseek   ( stk_fs_t *fsi, int fd, off_t offset, int flag )
      {
	  int ret ;

	  XPN_DEBUG_BEGIN_CUSTOM("%d %d %d", fd, offset, flag) ;

          /* check params */
          if ( ! xpni_fit_is_correct(fd) )
              return (-1) ;

          /* lseek file: data only */
	  ret = xpni_lowfsi_lseek(xpni_fit_get_XPN_DATA_FD(fd),offset,flag);

	  XPN_DEBUG_END_CUSTOM("%d %d %d", fd, offset, flag) ;

          /* return ok */
          return ret ;
      }

      ssize_t stk_mid_ft_write ( stk_fs_t *fsi, int fd, void *buffer, size_t size )
      {
          char *fmeta_fsTag;     /* File metadata (file system tag) */
          int   fmeta_nerrors;   /* File metadata (number of errors) */
          ssize_t   ret;         /* Returned value from called functions */

	  XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;

          /* check params */
          if ( ! xpni_fit_is_correct(fd) )
              return (-1) ;

          /* Select (file system type) x (status) */
          fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
          fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

          ret = -1 ;
          if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
	  {
		switch (fmeta_nerrors)
		{
			case 0:
                             //ret = stk_mid_ft_swrite_fail_r5o(fd,buffer,(int)offset,size) ;
                             ret = stk_mid_ft_swrite_nofail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			case 1:
                             ret = stk_mid_ft_swrite_fail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			default:
                             ret = (-1);
			     break;
		}
	  }

	  XPN_DEBUG_END_CUSTOM("%d %p %d -> %d", fd, buffer, size, ret) ;

          /* Return bytes written */
          return ret ;
      }

      ssize_t stk_mid_ft_read ( stk_fs_t *fsi, int fd, void *buffer, size_t size )
      {
          char *fmeta_fsTag;     /* File metadata (file system tag) */
          int   fmeta_nerrors;   /* File metadata (number of errors) */
          ssize_t   ret;         /* Returned value from called functions */

	  XPN_DEBUG_BEGIN_CUSTOM("%d %p %d", fd, buffer, size) ;

          /* check params */
          if ( ! xpni_fit_is_correct(fd) )
               return (-1) ;

          /* Select file system type */
          fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
          fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

	  ret = -1 ;
          if (! strncmp(fmeta_fsTag, FS_TAG_RAID5OUTER, strlen(FS_TAG_RAID5OUTER)))
	  {
		switch (fmeta_nerrors)
		{
			case 0:
                             //ret = stk_mid_ft_sread_fail_r5o(fd,buffer,offset,size) ;
                             ret = stk_mid_ft_sread_nofail_r5o(fd,buffer,offset,size) ;
			     break;

			case 1:
                             ret = stk_mid_ft_sread_fail_r5o(fd,buffer,offset,size) ;
			     break;

			default:
                             ret = -1;
			     break;
		}
	  }

	  XPN_DEBUG_END_CUSTOM("%d %p %d -> %d", fd, buffer, size, ret) ;

          /* Return bytes readed */
          return ret ;
      }


      //
      // import + export
      //

      int stk_mid_ft_exportFile ( int fd )
      {
          char *fmeta_fsTag;     /* File metadata (file system tag) */
          ssize_t ret;           /* Returned value from called functions */

	  XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

          /* check params */
          if ( ! xpni_fit_is_correct(fd) )
               return (-1) ;

          /* Select file system type */
          fmeta_fsTag = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;

	  ret = -1 ;
          if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER))) {
              ret = stk_mid_ft_exportFile_r5o(fd) ;
	  }

	  XPN_DEBUG_END_CUSTOM("%d", fd) ;

          /* Return bytes readed */
          return ret ;
      }

      int stk_mid_ft_importFile ( int fd )
      {
          char *fmeta_fsTag;     /* File metadata (file system tag) */
          ssize_t ret;           /* Returned value from called functions */

	  XPN_DEBUG_BEGIN_CUSTOM("%d", fd) ;

          /* check params */
          if ( ! xpni_fit_is_correct(fd) )
               return (-1) ;

          /* Select file system type */
          fmeta_fsTag = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;

	  ret = -1 ;
          if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER))) {
              ret = stk_mid_ft_importFile_r5o(fd) ;
	  }

	  XPN_DEBUG_END_CUSTOM("%d", fd) ;

          /* Return bytes readed */
          return ret ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR   *stk_mid_ft_opendir ( stk_fs_t *fsi, char * path )
      {
          DIR  *ret;

	  XPN_DEBUG_BEGIN_CUSTOM("%s", path) ;
	  ret = STK_FS_OPENDIR(path);
       // !! xpni_fit_set_XPN_DATA_FD(dd, (long)ret) ; /* sizeof(int) === sizeof(DIR *) */
	  XPN_DEBUG_END_CUSTOM("%s", path) ;

	  return ret;
      }

      int     stk_mid_ft_closedir  ( stk_fs_t *fsi, DIR *dirp )
      {
          int  ret;

	  XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;
	  ret = STK_FS_CLOSEDIR(dirp);
	  XPN_DEBUG_END_CUSTOM("%p", dirp) ;

	  return ret;
      }

      struct dirent *stk_mid_ft_readdir ( stk_fs_t *fsi, DIR *dirp )
      {
          struct dirent *ret;

	  XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;
	  ret = xpni_lowfsi_readdir(dirp);
	  XPN_DEBUG_END_CUSTOM("%p", dirp) ;

	  return ret;
      }

      void    stk_mid_ft_rewinddir ( stk_fs_t *fsi, DIR *dirp )
      {
	  XPN_DEBUG_BEGIN_CUSTOM("%p", dirp) ;
	  xpni_lowfsi_readdir(dirp);
	  XPN_DEBUG_END_CUSTOM("%p", dirp) ;
      }


      long    stk_mid_ft_mkdir     ( stk_fs_t *fsi, const char *pathname, int mode )
      {
          long  ret;

	  XPN_DEBUG_BEGIN_CUSTOM("%s %d", pathname, mode) ;
	  ret = xpni_lowfsi_mkdir(pathname, mode);
	  XPN_DEBUG_END_CUSTOM("%s %d", pathname, mode) ;

	  return ret;
      }

      long    stk_mid_ft_rmdir     ( stk_fs_t *fsi, const char *pathname )
      {
          long  ret;

	  XPN_DEBUG_BEGIN_CUSTOM("%s", pathname) ;
	  ret = xpni_lowfsi_rmdir(pathname);
	  XPN_DEBUG_END_CUSTOM("%s", pathname) ;

	  return ret;
      }

   /* ................................................................... */

