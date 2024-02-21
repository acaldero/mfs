
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


   /* ... Include / Inclusion ........................................... */

      #include "include/util_log_api.h"


   /* ... Variables / Variabls .......................................... */

      FILE            *xpni_log_lfile ;
      pthread_mutex_t  xpni_log_fastmutex = PTHREAD_MUTEX_INITIALIZER ;


   /* ... Functions / Funciones ......................................... */

      int util_log_elog
      (
         /*IN*/  struct timeval *t1,
         /*IN*/  struct timeval *t2,
         /*IN*/  char           *action,
         /*IN*/  int             object,
         /*IN*/  int             offset,
         /*IN*/  int             size
      )
      {

         struct timeval  diff ;
         float           diff_ms ;
         float           t1_ms ;
	 /*
         long            t1_s, t1_m, t1_h, t1_D, t1_Y ;
	 */
	 char           *object_name ;
	 int             ret ;


         /* debugging */
	 XPN_DEBUG_BEGIN_CUSTOM("%p,%p,%s,%d,%d,%d", t1,t2,action,object,offset,size) ;

	 /* get time as msec ... */
         TIME_MISC_DiffTime(t1,t2,&diff);
         diff_ms = TIME_MISC_TimevaltoMicro(&diff);
         t1_ms   = TIME_MISC_TimevaltoMicro(t1);

	 /*
	 t1_s  = t1_ms / 1000000 ;
	 t1_m  = t1_s  / 60 ;
	         t1_s = t1_s - t1_m*60 ;
	 t1_h  = t1_m  / 60 ;
	         t1_m = t1_m - t1_h*60 ;
	 t1_D  = t1_h  / 24 ;
	         t1_h = t1_h - t1_D*24 ;
	 t1_Y  = t1_D  / 365.25 ;
	         t1_D = t1_D - t1_Y*365.25 ;
	 t1_Y  = t1_Y + 1970 ;
	 */

	 /* get object name... */
	 if (-1 != object)
             object_name = xpni_fit_get_XPN_FNAME(object) ;
	 else
             object_name = "null";

	 /* write event to log... */
         ret = util_log_vprintf("%f %f %d %s %d %d %s\n",
				 t1_ms,diff_ms,
                                 object,action,offset,size,
				 object_name) ;
	 /*
         ret = util_log_vprintf("%4.0ld-%2.0ld-%2.0ld-%2.0ld-%2.0ld %f %d %s %d %d %s\n",
                                 t1_Y,t1_D,t1_h,t1_m,t1_s,diff_ms,
                                 object,action,offset,size,
				 object_name) ;
	 */

         /* debugging */
	 XPN_DEBUG_END_CUSTOM("%p,%p,%s,%d,%d,%d", t1,t2,action,object,offset,size) ;

	 /* return 'ret' */
         return ret ;

      }

      int util_log_vprintf
      (
         /*IN*/   char *einfo,
         ...
      )
      {

        va_list  vl ;
	char    *fname ;
	int      ret ;


        va_start(vl,einfo) ;

        /* debugging */
	XPN_DEBUG_BEGIN_CUSTOM("%s", einfo) ;

	/* record event */
	pthread_mutex_lock(&xpni_log_fastmutex) ;

	   if (NULL == xpni_log_lfile)
	   {
	       fname = STRING_MISC_Dsprintf("xpni_log.%d",getpid()) ;
	       xpni_log_lfile = fopen(fname,"a") ;
	       free(fname) ;
	   }

	   if (NULL != xpni_log_lfile)
	   {
	       ret = vfprintf(xpni_log_lfile,einfo,vl) ;
	   }

	pthread_mutex_unlock(&xpni_log_fastmutex) ;

        /* debugging */
	XPN_DEBUG_END_CUSTOM("%s", einfo) ;

	/* return 'ret' */
        va_end(vl) ;
	return ret;

      }


   /* ................................................................... */

