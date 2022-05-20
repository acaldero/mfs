
#ifndef _STK_MID_FT_H
#define _STK_MID_FT_H

 #ifdef  __cplusplus
    extern "C" {
 #endif


   /* ... Include / Inclusion ........................................... */

      #include "base_lib.h"
      #include "base/math_misc.h"
      #include "include/stk_fd.h"
      #include "include/stk_fs.h"


   /* ... Const / Constantes ............................................ */

      #define FS_TAG_RAID5OUTER  "stk_mid_ft_raid5outer"
      #define FS_TAG_RAID5INNER  "stk_mid_ft_raid5inner"


   /* ... Functions / Funciones ......................................... */

      // Init + finalize
      int     stk_mid_ft_init      ( stk_fs_t *fsi, stk_fs_t *low_fsi ) ;
      int     stk_mid_ft_finalize  ( stk_fs_t *fsi ) ;

      // File API
      int     stk_mid_ft_creat     ( stk_fs_t *fsi, char *path, mode_t mode ) ;
      int     stk_mid_ft_open      ( stk_fs_t *fsi, char *path, int flags, mode_t mode ) ;
      int     stk_mid_ft_close     ( stk_fs_t *fsi, int fd ) ;
      off_t   stk_mid_ft_lseek     ( stk_fs_t *fsi, int fd, off_t offset, int flag ) ;
      ssize_t stk_mid_ft_write     ( stk_fs_t *fsi, int fd, void *buffer, size_t size ) ;
      ssize_t stk_mid_ft_read      ( stk_fs_t *fsi, int fd, void *buffer, size_t size ) ;

      // Directory API
      long    stk_mid_ft_mkdir          ( stk_fs_t *fsi, const char *pathname, int mode ) ;
      long    stk_mid_ft_rmdir          ( stk_fs_t *fsi, const char *pathname ) ;

      DIR   * stk_mid_ft_opendir        ( stk_fs_t *fsi, char *path ) ;
      int     stk_mid_ft_closedir       ( stk_fs_t *fsi, DIR *dirp ) ;
      struct dirent *stk_mid_ft_readdir ( stk_fs_t *fsi, DIR *dirp ) ;
      void    stk_mid_ft_rewinddir      ( stk_fs_t *fsi, DIR *dirp ) ;


   /* ................................................................... */


 #ifdef  __cplusplus
    }
 #endif

#endif

