
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

 #include "mfs_params.h"

 int  mfs_params_show ( params_t *params )
 {
 	printf(" Current parameters:\n");
      	printf(" | -d <base directory>:  \t'%s'\n",   params->data_prefix) ;
      	printf(" | -f (POSIX | MPI-IO):  \t%s\n",     params->file_backend_name) ;
      	printf(" | -i (POSIX):  \t\t%s\n",            params->directory_backend_name) ;
      	printf(" | -b (GDBM):  \t\t\t%s\n",           params->dbm_backend_name) ;
      	printf(" | -t (ondemand | pool):  \t'%s'\n",  params->thread_launch_name) ;
      	printf(" | -n <partition file>:  \t'%s'\n",   params->conf_fname) ;

      	// return OK
      	return 1;
 }

 void mfs_params_show_usage ( void )
 {
      	printf("Usage:\n");
      	printf("\t-d <string>:  name of the base directory\n") ;
      	printf("\t-f <string>:  POSIX | MPI-IO\n") ;
      	printf("\t-i <string>:  POSIX\n") ;
      	printf("\t-b <string>:  GDBM\n") ;
      	printf("\t-t <string>:  ondemand | pool\n") ;
      	printf("\t-n <string>:  name of the partition file\n") ;
 }

 struct option long_options[] =
        {
          { "verbose",            no_argument,       NULL, 'v' },
          { "base_directory",     required_argument, NULL, 'd' },
          { "file_backend",       required_argument, NULL, 'f' },
          { "directory_backend",  required_argument, NULL, 'i' },
          { "dbm_backend",        required_argument, NULL, 'b' },
          { "thread_backend",     required_argument, NULL, 't' },
          { "process_in_server",  required_argument, NULL, 'n' },
          { 0 }
        } ;


 int mfs_params_get ( params_t *params, int *argc, char ***argv )
 {
        int   c = -1 ;
	int   option_index = 0;
	char *short_opt = NULL ;

      	// set default values
      	params->argc = argc ;
      	params->argv = argv ;

        params->mfs_server_stub_pname = strdup(DEFAULT_STUB_PNAME) ;
        params->data_prefix = strdup(DEFAULT_DATA_PREFIX) ;
        params->conf_fname  = strdup(DEFAULT_CONF_FILE) ;
	params->server_port = 12345 ; // TODO: get from command-line ??

        params->file_backend           = FILE_USE_POSIX ;
        params->file_backend_name      = strdup("POSIX") ;
        params->dbm_backend            = DBM_USE_GDBM ;
        params->dbm_backend_name       = strdup("GDBM") ;
        params->directory_backend      = DIRECTORY_USE_POSIX ;
        params->directory_backend_name = strdup("POSIX") ;
        params->thread_launch          = THREAD_USE_ONDEMAND ;
        params->thread_launch_name     = strdup("On demand") ;
        params->comm_backend           = COMM_USE_MPI ;
        params->comm_backend_name      = strdup("MPI") ;
        params->ns_backend             = NS_USE_DBM ;
        params->ns_backend_name        = strdup("DBM") ;

	// getopt_long...
	short_opt = "vd:n:f:i:b:t:" ;
        c = getopt_long(*argc, *argv, short_opt, long_options, &option_index) ;
	while (c != -1)
	{
           switch (c)
           {
             case 'v':
		  // verbose
                  break ;

	     case 'd':
		  mfs_free_and_strdup(&(params->data_prefix), optarg) ;
                  break ;

	     case 'n':
		  mfs_free_and_strdup(&(params->conf_fname), optarg) ;
                  break ;

	     case 'f':
		  if (!strcmp("POSIX",  optarg)) {
		      params->file_backend = FILE_USE_POSIX ;
		      mfs_free_and_strdup(&(params->file_backend_name), "POSIX") ;
		  }
		  if (!strcmp("MPI-IO", optarg)) {
		      params->file_backend = FILE_USE_MPI_IO ;
		      mfs_free_and_strdup(&(params->file_backend_name), "MPI-IO") ;
		  }
                  break ;

	     case 'b':
		  if (!strcmp("GDBM",  optarg)) {
		      params->dbm_backend = DBM_USE_GDBM ;
		      mfs_free_and_strdup(&(params->dbm_backend_name), "GDBM") ;
		  }
		  if (!strcmp("REDIS", optarg)) {
		      params->dbm_backend = DBM_USE_REDIS ;
		      mfs_free_and_strdup(&(params->dbm_backend_name), "REDIS") ;
		  }
                  break ;

	     case 'i':
		  if (!strcmp("POSIX",  optarg)) {
		      params->directory_backend = DIRECTORY_USE_POSIX ;
		      mfs_free_and_strdup(&(params->directory_backend_name), "POSIX") ;
		  }
                  break ;

	     case 't':
		  if (!strcmp("ondemand", optarg) ) {
		      params->thread_launch = THREAD_USE_ONDEMAND ;
		      mfs_free_and_strdup(&(params->thread_launch_name), "On Demand") ;
		  }
		  if (!strcmp("pool",     optarg) ) {
		      params->thread_launch = THREAD_USE_POOL ;
		      mfs_free_and_strdup(&(params->thread_launch_name), "Pool") ;
		  }
                  break ;

             case '?':
                  mfs_params_show_usage() ;
                  exit(-1) ;
                  break ;

             default:
                  break ;
           }

           c = getopt_long(*argc, *argv, short_opt, long_options, &option_index) ;
	}

      	// return OK
      	return 1;
}

int  mfs_params_free       ( params_t *params )
{
        // Free dynamic memory
        free(params->data_prefix) ;
        free(params->conf_fname) ;
        free(params->file_backend_name) ;
        free(params->dbm_backend_name) ;
        free(params->directory_backend_name) ;
        free(params->thread_launch_name) ;
        free(params->comm_backend_name) ;
        free(params->mfs_server_stub_pname) ;
        free(params->ns_backend_name) ;

      	// return OK
      	return 1;
}

