
#ifndef _XPNI_LOWFSI_H
#define _XPNI_LOWFSI_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "all_system.h"


   /* ... Macros / Macros ............................................... */

#if defined(XPNI_LOWFSI_XPN)

      #define xpni_lowfsi_rename(msrc,mdst) \
              xpn_rename((msrc),(mdst))

      #define xpni_lowfsi_unlink(mfname) \
              xpn_unlink((mfname))

#elif defined(XPNI_LOWFSI_POSIX)

      #define xpni_lowfsi_rename(msrc,mdst) \
              rename((msrc),(mdst))

      #define xpni_lowfsi_unlink(mfname) \
              unlink((mfname))

#elif defined(XPNI_LOWFSI_PRINTF)

      #define xpni_lowfsi_init() \
              printf("XPNI_LOWFSI_INIT()\n")

      #define xpni_lowfsi_destroy() \
              printf("XPNI_LOWFSI_DESTROY()\n")

      #define xpni_lowfsi_open(mfname,mflag,mmode) \
              printf("XPNI_LOWFSI_OPEN(%s,%d,%d)\n",mfname,mflag,mmode)

      #define xpni_lowfsi_creat(mfname,mmode) \
              printf("XPNI_LOWFSI_CREAT(%s,%d)\n",mfname,mmode)

      #define xpni_lowfsi_close(mfd) \
              printf("XPNI_LOWFSI_CLOSE(%d)\n",mfd)

      #define xpni_lowfsi_sread(mfd,mbuf,moff,msiz) \
              printf("XPNI_LOWFSI_SREAD(%d,%p,%d,%d)\n",mfd,mbuf,moff,msiz)

      #define xpni_lowfsi_swrite(mfd,mbuf,moff,msiz) \
              printf("XPNI_LOWFSI_SWRITE(%d,%p,%d,%d)\n",mfd,mbuf,moff,msiz)

      #define xpni_lowfsi_rename(msrc,mdst) \
              printf("XPNI_LOWFSI_RENAME(%s,%s)\n",msrc,mdst)

      #define xpni_lowfsi_unlink(mfname) \
              printf("XPNI_LOWFSI_UNLINK(%s)\n",mfname)

      #define xpni_lowfsi_lseek(mfd,moffset,mflag) \
              printf("XPNI_LOWFSI_LSEEK(%d,%d,%d)\n",mfd,moffset,mflag)

      #define xpni_lowfsi_opendir(mpath) \
              printf("XPNI_LOWFSI_OPENDIR(%s)\n",mpath)

#endif


  /* .................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

