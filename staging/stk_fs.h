
#ifndef _STK_FS_H
#define _STK_FS_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"


   /* ... Types / Tipos ................................................. */

      typedef 
      struct struct_stk_fs_t stk_fs_t ;

      struct struct_stk_fs_t
      {

	 // internal data
         char     *fs_name ;
         stk_fs_t *low_fsi ;

	 // API
         int     (*stk_init)       ( stk_fs_t *fsi, stk_fs_t *low_fsi ) ;
         int     (*stk_finalize)   ( stk_fs_t *fsi ) ;

         int     (*stk_creat)      ( char *path, mode_t mode ) ;
         int     (*stk_open)       ( char *path, int flags, mode_t mode ) ;
         int     (*stk_close)      ( int fd ) ;
         off_t   (*stk_lseek)      ( int fd, off_t offset, int flag ) ;
         ssize_t (*stk_write)      ( int fd, void *buffer, size_t size ) ;
         ssize_t (*stk_read)       ( int fd, void *buffer, size_t size ) ;

         long    (*stk_mkdir)          ( const char *pathname, int mode ) ;
         long    (*stk_rmdir)          ( const char *pathname ) ;
         DIR   * (*stk_opendir)        (       char *path ) ;
         int     (*stk_closedir)       (       DIR  *dirp ) ;
         struct dirent *(*stk_readdir) (       DIR  *dirp ) ;
         void    (*stk_rewinddir)      (       DIR  *dirp ) ;

      } ;


   /* ... Functions / Funciones ......................................... */

      int       stk_fs_init     ( stk_fs_t *fsi, stk_fs_t *low_fsi ) ;
      int       stk_fs_finalize ( stk_fs_t *fsi ) ;

      stk_fs_t *stk_fs_new      ( void ) ;
      int       stk_fs_destroy  ( stk_fs_t **fsi ) ;


   /* ... Macros / Macros ............................................... */

      #define STK_FS_GET_FSNAME(fd) \
              ((fsi) == NULL) ? NULL : ((fsi)->fs_name)

      #define STK_FS_GET_LOWFSI(fsi) \
              ((fsi) == NULL) ? NULL : ((fsi)->low_fsi)


      #define STK_FS_INIT(fsi,low_fsi) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_init)((fsi),(low_fsi))

      #define STK_FS_FINALIZE(fsi) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_finalize)((fsi))


      #define STK_FS_CREAT(fsi,path,mode) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_creat)((path),(mode))

      #define STK_FS_OPEN(fsi,path,flags,mode) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_open)((path),(flags),(mode))

      #define STK_FS_CLOSE(fsi,fd) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_close)((fd))

      #define STK_FS_LSEEK(fsi,fd,offset,flag) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_lseek)((fd),(offset),(flag))

      #define STK_FS_WRITE(fsi,fd,buffer,size) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_write)((fd),(buffer),(size))

      #define STK_FS_READ(fsi,fd,buffer,size) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_read)((fd),(buffer),(size))


      #define STK_FS_MKDIR(fsi,pathname,mode) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_mkdir)((pathname),(mode))

      #define STK_FS_RMDIR(fsi,pathname) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_rmdir)((pathname))

      #define STK_FS_OPENDIR(fsi,path) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_opendir)((path))

      #define STK_FS_CLOSEDIR(fsi,dirp) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_closedir)((dirp))

      #define STK_FS_READDIR(fsi,dirp) \
              ((fsi) == NULL) ? NULL : ((fsi)->stk_readdir)((dirp))

      #define STK_FS_REWINDDIR(fsi,dirp) \
              ((fsi) == NULL) ? -1 : ((fsi)->stk_rewinddir)((dirp))


   /* ................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

