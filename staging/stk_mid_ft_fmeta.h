
#ifndef _STK_MID_FT_FMETA_H_
#define _STK_MID_FT_FMETA_H_

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "include/stk_fs.h"


   /* ... Types / Tipos ................................................. */

      struct struct_stk_mid_ft_fmeta_t
      {
	 // lower file system references
         char     *xpn_fname ;
         int       xpn_fd[2] ;
         int       xpni_fsd ;
         int       xpn_flag ;
         mode_t    xpn_mode ;

	 // internal data
         int       block_size;
         int       servers_count;
         int       start_server;
         char     *filesystem_tag;
         char     *partition_name;
         int       nerrors;
         int      *servers;
         int      *servers_status;
         int      *Ld;
         int      *Lr;
         int      *Ld2r;
         int      *Lr2d;

	 // associated file system
         stk_fs_t *xpn_fs ;
      } ;

      typedef 
      struct struct_stk_mid_ft_fmeta_t stk_mid_ft_fmeta_t ;


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
      ) ;

      int stk_mid_ft_fmeta_finalize   ( fmeta_t *fmeta ) ;


   /* ... Macros / Macros ............................................... */

      /* Checking */
      #define stk_mid_ft_fmeta_is_correct(mfd)  (NULL != (mfd))


      /* Access to field */
      #define stk_mid_ft_fmeta_get_FNAME(mfd)     ((mfd)->xpn_fname)
      #define stk_mid_ft_fmeta_get_DATA_FD(mfd)   ((mfd)->xpn_fd[0])
      #define stk_mid_ft_fmeta_get_MDATA_FD(mfd)  ((mfd)->xpn_fd[1])
      #define stk_mid_ft_fmeta_get_FMETA(mfd)     ((mfd)->xpn_fmeta)
      #define stk_mid_ft_fmeta_get_FSD(mfd)       ((mfd)->xpni_fsd)
      #define stk_mid_ft_fmeta_get_FLAG(mfd)      ((mfd)->xpn_flag)
      #define stk_mid_ft_fmeta_get_MODE(mfd)      ((mfd)->xpn_mode)


      /* Modify a field */
      #define stk_mid_ft_fmeta_set_FNAME(mfd,mfname)     (((mfd)->xpn_fname)=(mfname))
      #define stk_mid_ft_fmeta_set_DATA_FD(mfd,mpnfd)    (((mfd)->xpn_fd[0])=(mpnfd))
      #define stk_mid_ft_fmeta_set_MDATA_FD(mfd,mpnfd)   (((mfd)->xpn_fd[1])=(mpnfd))
      #define stk_mid_ft_fmeta_set_FMETA(mfd,mfmeta)     (((mfd)->xpn_fmeta)=(mfmeta))
      #define stk_mid_ft_fmeta_set_FSD(mfd,mfsd)         (((mfd)->xpni_fsd)=(mfsd))
      #define stk_mid_ft_fmeta_set_FLAG(mfd,mflag)       (((mfd)->xpn_flag)=(mflag))
      #define stk_mid_ft_fmeta_set_MODE(mfd,mmode)       (((mfd)->xpn_mode)=(mmode))


   /* ................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

