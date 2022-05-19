
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

	 // API
         int     (*stk_register)   ( stk_fs_t *fsi ) ;
         int     (*stk_unregister) ( stk_fs_t *fsi ) ;

         int     (*stk_init)       ( void ) ;
         int     (*stk_destroy)    ( void ) ;

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

      stk_fs_t  *stk_fs_new      ( void )
      int        stk_fs_destroy  ( stk_fs_t **fsi )


   /* ................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

