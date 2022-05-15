

   /* ... Include / Inclusion ........................................... */

      #include "include/xpn_si_ft.h"


   /* ... Internal functions / Funciones internas ....................... */

      char *xpnsi_ft_metaData_getMetaDataFileName ( int fd )
      {
        char  *pfname ;
        int    pfname_len ;

        /* debugging */
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_metaData_getMetaDataFileName(%d); \n",
                   __FILE__,__LINE__,fd);
        #endif

        pfname_len = strlen(xpni_fit_get_XPN_FNAME(fd)) +
                     strlen(".xpnsi_ft") +
                     1 /* '\0'*/ ;

        pfname = (char *)malloc(pfname_len) ;
        if (NULL == pfname)
 	    return NULL ;

	strcpy(pfname,xpni_fit_get_XPN_FNAME(fd)) ;
	strcat(pfname,".xpnsi_ft") ;

        return pfname ;
      }

      ssize_t xpnsi_ft_sread_fail_r5o
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
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_sread_r5o(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif


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

	return ret ;
      }

      ssize_t xpnsi_ft_sread_nofail_r5o
      (
         int fd,
         void *buffer,
         off_t offset,
         size_t size
      )
      {
        /* debugging */
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_sread_r5o(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif

	return xpni_lowfsi_pread(xpni_fit_get_XPN_DATA_FD(fd),
	 		         buffer,
			         (int)offset,
			         size) ;
      }

      ssize_t xpnsi_ft_sread_r5o
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
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_sread_r5o(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* Select (file system type) x (status) */
        fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
        fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

        if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
	{
		switch (fmeta_nerrors)
		{
			case 0:
                             ret = xpnsi_ft_sread_nofail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			case 1:
                             ret = xpnsi_ft_sread_fail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			default:
                             ret = (-1);
#if defined(XPNI_DEBUG)
                             printf("[%s:%d] xpnsi_ft_sread_r5o(%d,%p,%d,%d): %d fail(s)\n",
                                    __FILE__,__LINE__,fd,buffer,(int)offset,size,fmeta_nerrors);
#endif
			     break;
		}
	}

        else
	{
            ret = (-1);
#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_sread_r5o(%d,%p,%d,%d): Unknow file system tag: '%s' (not %s or %s)\n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size,
		   fmeta_fsTag,FS_TAG_RAID5OUTER,FS_TAG_RAID5OUTER);
#endif
	}

        /* Return bytes written */
        return ret ;
      }


      ssize_t xpnsi_ft_swrite_nofail_r5o
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
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_swrite_r5o(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif


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

        buffer_i = (char *)buffer;

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


        /*
         * Return bytes readed
         */

        return size ;
      }

      ssize_t xpnsi_ft_swrite_fail_r5o
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
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_swrite_r5o(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif


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

        buffer_i = (char *)buffer;

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


        /*
         * Return bytes readed
         */

        return size ;
      }

      ssize_t xpnsi_ft_swrite_r5o
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
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_swrite_r5o(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* Select (file system type) x (status) */
        fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
        fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

        if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
	{
		switch (fmeta_nerrors)
		{
			case 0:
                             ret = xpnsi_ft_swrite_nofail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			case 1:
                             ret = xpnsi_ft_swrite_fail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			default:
                             ret = (-1);
#if defined(XPNI_DEBUG)
                             printf("[%s:%d] xpnsi_ft_swrite_r5o(%d,%p,%d,%d): %d fail(s)\n",
                                    __FILE__,__LINE__,fd,buffer,(int)offset,size,fmeta_nerrors);
#endif
			     break;
		}
	}

        else
	{
            ret = (-1);
#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_swrite_r5o(%d,%p,%d,%d): Unknow file system tag: '%s' (not %s or %s)\n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size,
		   fmeta_fsTag,FS_TAG_RAID5OUTER,FS_TAG_RAID5OUTER);
#endif
	}

        /* Return bytes written */
        return ret ;
      }

      int xpnsi_ft_exportFile_r5o ( int fd )
      {
	char  *pfname ;
	int    ret ;

        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_exportFile_r5o(%d); \n",
                   __FILE__,__LINE__,fd);
	#endif

        /* parity file name */
        pfname = xpnsi_ft_metaData_getMetaDataFileName(fd) ;
        if (NULL == pfname)
	    return (-1) ;

        /* unlink file */
	ret = xpn_unlink(pfname) ;

        /* free parity file name */
        free(pfname) ;

	/* return 'ok' */
	return (ret) ;
      }

      int xpnsi_ft_importFile_r5o ( int fd )
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
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_importFile_r5o(%d); \n",
                   __FILE__,__LINE__,fd);
	#endif

        /* parity file name */
	pfname = xpnsi_ft_metaData_getMetaDataFileName(fd) ;
        if (NULL == pfname)
 	    return (-1) ;

        /* open parity file */
        ret = xpni_lowfsi_open(pfname,
			       O_CREAT|O_RDWR,
			       S_IRWXU);
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

	/* return ok */
	return (1) ;

      }


   /* ... Functions / Funciones ......................................... */

      //
      // register + unregister
      //

      int xpnsi_ft_register   ( xpni_fsi_t *fsi )
      {
	/* register xpnsi_ft interface */
	fsi->fsi_name   = STRING_MISC_StrDup("xpn_si_ft") ;

	// file API
	fsi->fsi_init       = xpnsi_ft_init ;
        fsi->fsi_destroy    = xpnsi_ft_destroy ;
        fsi->fsi_open       = xpnsi_ft_open ;
        fsi->fsi_creat      = xpnsi_ft_creat ;
        fsi->fsi_close      = xpnsi_ft_close ;
        fsi->fsi_read       = xpnsi_ft_read ;
        fsi->fsi_write      = xpnsi_ft_write ;
        fsi->fsi_lseek      = xpnsi_ft_lseek ;
	// directory API
        fsi->fsi_opendir    = xpnsi_ft_opendir ;
        fsi->fsi_closedir   = xpnsi_ft_closedir ;
        fsi->fsi_readdir    = xpnsi_ft_readdir ;
        fsi->fsi_mkdir      = xpnsi_ft_mkdir ;
        fsi->fsi_rmdir      = xpnsi_ft_rmdir ;
        fsi->fsi_rewinddir  = xpnsi_ft_rewinddir ;
	// register/unregister API
        fsi->fsi_register   = xpnsi_ft_register ;
        fsi->fsi_unregister = xpnsi_ft_unregister ;

	/* return ok */
	return (1) ;
      }

      int xpnsi_ft_unregister ( xpni_fsi_t *fsi )
      {
	/* check params */
	if (NULL == fsi) {
	    return -1 ;
	}

	/* unregister xpnsb_posix interface */
	if (NULL != fsi->fsi_name) {
	    free(fsi->fsi_name) ;
	    fsi->fsi_name   = NULL ;
	}

	// file API
	fsi->fsi_init       = NULL ;
        fsi->fsi_destroy    = NULL ;
        fsi->fsi_open       = NULL ;
        fsi->fsi_creat      = NULL ;
        fsi->fsi_close      = NULL ;
        fsi->fsi_read       = NULL ;
        fsi->fsi_write      = NULL ;
        fsi->fsi_lseek      = NULL ;
	// directory API
        fsi->fsi_opendir    = NULL ;
        fsi->fsi_closedir   = NULL ;
        fsi->fsi_readdir    = NULL ;
        fsi->fsi_mkdir      = NULL ;
        fsi->fsi_rmdir      = NULL ;
        fsi->fsi_rewinddir  = NULL ;
	// register/unregister API
        fsi->fsi_register   = NULL ;
        fsi->fsi_unregister = NULL ;

	/* return ok */
	return (1) ;
      }


      //
      // init + destroy
      //

      int xpnsi_ft_init ()
      {
        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_init(); \n",
                   __FILE__,__LINE__);
	#endif

	return xpni_lowfsi_init();
      }

      int xpnsi_ft_destroy ()
      {
        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_destroy(); \n",
                   __FILE__,__LINE__);
	#endif

	/* TODO: bug in xpn_destroy */
#if (0)
	return xpni_lowfsi_destroy();
#else
	return (1);
#endif

      }


      //
      // open + close + sread + swrite
      //

      int xpnsi_ft_creat ( int fd )
      {
       char      *pfname ;
       int        ret ;

       /* check params */
       if ( ! xpni_fit_is_correct(fd) )
           return (-1) ;

       /* parity file name */
       if ( ! strcmp
              (
                (xpni_fit_get_XPN_FMETA(fd)).filesystem_tag,
		FS_TAG_RAID5OUTER
              )
       )
       {
	       /* parity file name */
               pfname = xpnsi_ft_metaData_getMetaDataFileName(fd) ;
	       if (NULL == pfname)
		   return (-1) ;

	       /* open parity file */
	       ret = xpni_lowfsi_open(pfname,
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
       ret = xpni_lowfsi_creat(xpni_fit_get_XPN_FNAME(fd),
                               xpni_fit_get_XPN_MODE(fd)) ;
       if (ret < 0)
       {
           xpni_lowfsi_close(xpni_fit_get_XPN_MDATA_FD(fd)) ;
           return (-1) ;
       }

       /* update file description */
       xpni_fit_set_XPN_DATA_FD(fd,ret) ;

       /* return xpn file descriptor */
       return ret ;
      }

      int xpnsi_ft_open ( int fd )
      {
       char      *pfname ;
       int        ret ;

       /* debugging */
       #if defined(XPNI_DEBUG)
           printf("[%s:%d] xpnsi_ft_open(%d); \n",
                  __FILE__,__LINE__,fd);
       #endif

       /* check params */
       if ( ! xpni_fit_is_correct(fd) )
           return (-1) ;

       /* parity file name */
       if ( ! strcmp
              (
                (xpni_fit_get_XPN_FMETA(fd)).filesystem_tag,FS_TAG_RAID5OUTER
              )
       )
       {
	       /* parity file name */
               pfname = xpnsi_ft_metaData_getMetaDataFileName(fd) ;
	       if (NULL == pfname)
		   return (-1) ;

	       /* open parity file */
	       ret = xpni_lowfsi_open(pfname,
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
       ret = xpni_lowfsi_open(xpni_fit_get_XPN_FNAME(fd),
                              xpni_fit_get_XPN_FLAG(fd),
                              xpni_fit_get_XPN_MODE(fd)) ;
       if (ret < 0)
       {
           xpni_lowfsi_close(xpni_fit_get_XPN_MDATA_FD(fd)) ;
           return (-1) ;
       }

       /* update file description */
       xpni_fit_set_XPN_DATA_FD(fd,ret) ;

       /* return xpn file descriptor */
       return ret ;
      }

      int xpnsi_ft_close ( int fd )
      {
        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_close(%d); \n",
                   __FILE__,__LINE__,fd);
	#endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* close file: data and metadata */
        if ( ! strcmp
              (
                (xpni_fit_get_XPN_FMETA(fd)).filesystem_tag,FS_TAG_RAID5OUTER
              )
        )
        {
	   xpni_lowfsi_close(xpni_fit_get_XPN_MDATA_FD(fd));
        }

	xpni_lowfsi_close(xpni_fit_get_XPN_DATA_FD(fd));

        /* return ok */
        return (1) ;
      }

      off_t   xpnsi_ft_lseek   ( int fd, off_t offset, int flag )
      {
        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_lseek(%d); \n",
                   __FILE__,__LINE__,fd);
	#endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* lseek file: data only */
	xpni_lowfsi_lseek(xpni_fit_get_XPN_DATA_FD(fd),offset,flag);

        /* return ok */
        return (1) ;
      }

      ssize_t xpnsi_ft_swrite
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
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_swrite(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* Select (file system type) x (status) */
        fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
        fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

        if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
	{
		switch (fmeta_nerrors)
		{
			case 0:
                             //ret = xpnsi_ft_swrite_fail_r5o(fd,buffer,(int)offset,size) ;
                             ret = xpnsi_ft_swrite_nofail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			case 1:
                             ret = xpnsi_ft_swrite_fail_r5o(fd,buffer,(int)offset,size) ;
			     break;

			default:
                             ret = (-1);
#if defined(XPNI_DEBUG)
                             printf("[%s:%d] xpnsi_ft_swrite(%d,%p,%d,%d): %d fail(s)\n",
                                    __FILE__,__LINE__,fd,buffer,(int)offset,size,fmeta_nerrors);
#endif
			     break;
		}
	}

        else
	{
            ret = (-1);
#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_swrite(%d,%p,%d,%d): Unknow file system tag: '%s' (not %s or %s)\n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size,
		   fmeta_fsTag,FS_TAG_RAID5INNER,FS_TAG_RAID5OUTER);
#endif
	}

        /* Return bytes written */
        return ret ;
      }

      ssize_t xpnsi_ft_sread (int fd, void *buffer, off_t offset, size_t size)
      {
        char *fmeta_fsTag;     /* File metadata (file system tag) */
        int   fmeta_nerrors;   /* File metadata (number of errors) */
        ssize_t   ret;         /* Returned value from called functions */


        /* debugging */
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_sread(%d,%p,%d,%d); \n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size);
        #endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* Select file system type */
        fmeta_fsTag   = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;
        fmeta_nerrors = xpni_fit_get_XPN_FMETA(fd).nerrors ;

        if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
	{
		switch (fmeta_nerrors)
		{
			case 0:
                             //ret = xpnsi_ft_sread_fail_r5o(fd,buffer,offset,size) ;
                             ret = xpnsi_ft_sread_nofail_r5o(fd,buffer,offset,size) ;
			     break;

			case 1:
                             ret = xpnsi_ft_sread_fail_r5o(fd,buffer,offset,size) ;
			     break;

			default:
                             ret = (-1);
#if defined(XPNI_DEBUG)
                             printf("[%s:%d] xpnsi_ft_sread(%d,%p,%d,%d): %d fail(s)\n",
                                    __FILE__,__LINE__,fd,buffer,(int)offset,size,fmeta_nerrors);
#endif
			     break;
		}
	}

        else
	{
            ret = (-1);
#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_sread(%d,%p,%d,%d): Unknow file system tag: '%s' (not %s or %s)\n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size,
		   fmeta_fsTag,FS_TAG_RAID5INNER,FS_TAG_RAID5OUTER);
#endif
	}

        /* Return bytes readed */
        return ret ;
      }


      //
      // import + export
      //

      int xpnsi_ft_exportFile ( int fd )
      {
        char *fmeta_fsTag;     /* File metadata (file system tag) */
        ssize_t ret;           /* Returned value from called functions */


        /* debugging */
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_exportFile(%d); \n",
                   __FILE__,__LINE__,fd);
        #endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* Select file system type */
        fmeta_fsTag = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;

        if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
            ret = xpnsi_ft_exportFile_r5o(fd) ;

        else
	{
            ret = (-1);
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_exportFile(%d,%p,%d,%d): Unknow file system tag: '%s' (not %s or %s)\n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size,fmeta_fsTag,FS_TAG_RAID5INNER,FS_TAG_RAID5OUTER);
        #endif
	}

        /* Return bytes readed */
        return ret ;
      }

      int xpnsi_ft_importFile ( int fd )
      {
        char *fmeta_fsTag;     /* File metadata (file system tag) */
        ssize_t ret;           /* Returned value from called functions */


        /* debugging */
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_importFile(%d); \n",
                   __FILE__,__LINE__,fd);
        #endif

        /* check params */
        if ( ! xpni_fit_is_correct(fd) )
            return (-1) ;

        /* Select file system type */
        fmeta_fsTag = xpni_fit_get_XPN_FMETA(fd).filesystem_tag ;

        if (!strncmp(fmeta_fsTag,FS_TAG_RAID5OUTER,strlen(FS_TAG_RAID5OUTER)))
            ret = xpnsi_ft_importFile_r5o(fd) ;

        else
	{
            ret = (-1);
        #if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_importFile(%d,%p,%d,%d): Unknow file system tag: '%s' (not %s or %s)\n",
                   __FILE__,__LINE__,fd,buffer,(int)offset,size,fmeta_fsTag,FS_TAG_RAID5INNER,FS_TAG_RAID5OUTER);
        #endif
	}

        /* Return bytes readed */
        return ret ;
      }


      //
      // opendir + closedir + readdir + rewind
      //

      DIR   *xpnsi_ft_opendir ( int dd )
      {
        DIR  *ret;

        /* debugging */
	#if defined(XPNI_DEBUG)
            printf("[%s:%d] xpnsi_ft_opendir(%d); \n",
                   __FILE__,__LINE__,dd);
	#endif

        /* open directory */
	ret = xpni_lowfsi_opendir(xpni_fit_get_XPN_FNAME(dd));

        /* update directory description */
        xpni_fit_set_XPN_DATA_FD(dd, (long)ret) ; /* sizeof(int) === sizeof(DIR *) */

	return ret;

      }


   /* ................................................................... */

