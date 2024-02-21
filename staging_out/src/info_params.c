
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

 #include "info_params.h"

 int  info_params_show ( params_t *params )
 {
 	printf(" Current parameters:\n");
      	printf(" | -v  %d \t\t <level>\n",                  params->verbose) ;
      	printf(" | -f  %s  \t\t POSIX | MPI-IO\n",          params->file_backend_name) ;
      	printf(" | -i  %s  \t\t POSIX\n",                   params->directory_backend_name) ;
      	printf(" | -b  %s  \t\t GDBM  | TDB\n",             params->dbm_backend_name) ;
      	printf(" | -t '%s' \t ondemand | pool\n",           params->thread_launch_name) ;
      	printf(" | -n '%s' \t <partition file>\n",          params->conf_fname) ;

      	// return OK
      	return 1;
 }

 void info_params_show_usage ( void )
 {
      	printf("Usage:\n");
      	printf("\t-v <number>:  verbose level              (default: %d)\n",   DEFAULT_VERBOSE_LEVEL) ;
      	printf("\t-c <string>:  MPI   | SOCKET | LOCAL     (default: %s)\n",   "MPI") ;
      	printf("\t-f <string>:  POSIX | MPI-IO             (default: %s)\n",   "POSIX") ;
      	printf("\t-i <string>:  POSIX                      (default: %s)\n",   "POSIX") ;
      	printf("\t-b <string>:  GDBM  | TDB                (default: %s)\n",   "GDBM") ;
      	printf("\t-t <string>:  ondemand | pool            (default: %s)\n",   "ondemand") ;
      	printf("\t-n <string>:  name of the partition file (default: '%s')\n", DEFAULT_CONF_FILE) ;
 }

 struct option long_options[] =
        {
          { "verbose",            required_argument, NULL, 'v' },
          { "file_backend",       required_argument, NULL, 'f' },
          { "directory_backend",  required_argument, NULL, 'i' },
          { "dbm_backend",        required_argument, NULL, 'b' },
          { "thread_backend",     required_argument, NULL, 't' },
          { "partition_file",     required_argument, NULL, 'n' },
          { 0 }
        } ;


 int info_params_get ( params_t *params, int *argc, char ***argv )
 {
        int   c = -1 ;
	int   option_index = 0;
	char *short_opt = NULL ;

      	// set default values
      	params->argc = argc ;
      	params->argv = argv ;

      	params->verbose                = 0 ;
        params->mfs_server_stub_pname  = base_strdup(DEFAULT_STUB_PNAME) ;
        params->conf_fname             = base_strdup(DEFAULT_CONF_FILE) ;

        params->file_backend           = FILE_USE_POSIX ;
        params->file_backend_name      = base_strdup("POSIX") ;
        params->dbm_backend            = DBM_USE_GDBM ;
        params->dbm_backend_name       = base_strdup("GDBM") ;
        params->directory_backend      = DIRECTORY_USE_POSIX ;
        params->directory_backend_name = base_strdup("POSIX") ;
        params->thread_launch          = THREAD_USE_ONDEMAND ;
        params->thread_launch_name     = base_strdup("On demand") ;
        params->ns_backend             = NS_USE_TDB ;
        params->ns_backend_name        = base_strdup("TDB") ;

	// getopt_long...
	short_opt = "v:b:f:i:n:t:" ;
        c = getopt_long(*argc, *argv, short_opt, long_options, &option_index) ;
	while (c != -1)
	{
           switch (c)
           {
             case 'v':
		  params->verbose = atoi(optarg) ;
                  break ;

	     case 'n':
		  base_free_and_strdup(&(params->conf_fname), optarg) ;
                  break ;

	     case 'f':
		  if (!strcmp("POSIX",  optarg)) {
		      params->file_backend = FILE_USE_POSIX ;
		      base_free_and_strdup(&(params->file_backend_name), "POSIX") ;
		  }
		  if (!strcmp("MPI-IO", optarg)) {
		      params->file_backend = FILE_USE_MPI_IO ;
		      base_free_and_strdup(&(params->file_backend_name), "MPI-IO") ;
		  }
                  break ;

	     case 'b':
		  if (!strcmp("GDBM",  optarg)) {
		      params->dbm_backend = DBM_USE_GDBM ;
		      base_free_and_strdup(&(params->dbm_backend_name), "GDBM") ;
		  }
		  if (!strcmp("TDB",  optarg)) {
		      params->dbm_backend = DBM_USE_GDBM ;
		      base_free_and_strdup(&(params->dbm_backend_name), "TDB") ;
		  }
		  if (!strcmp("REDIS", optarg)) {
		      params->dbm_backend = DBM_USE_REDIS ;
		      base_free_and_strdup(&(params->dbm_backend_name), "REDIS") ;
		  }
                  break ;

	     case 'i':
		  if (!strcmp("POSIX",  optarg)) {
		      params->directory_backend = DIRECTORY_USE_POSIX ;
		      base_free_and_strdup(&(params->directory_backend_name), "POSIX") ;
		  }
                  break ;

	     case 't':
		  if (!strcmp("ondemand", optarg) ) {
		      params->thread_launch = THREAD_USE_ONDEMAND ;
		      base_free_and_strdup(&(params->thread_launch_name), "On Demand") ;
		  }
		  if (!strcmp("pool",     optarg) ) {
		      params->thread_launch = THREAD_USE_POOL ;
		      base_free_and_strdup(&(params->thread_launch_name), "Pool") ;
		  }
                  break ;

             case '?':
                  info_params_show_usage() ;
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

int  info_params_free       ( params_t *params )
{
        // Free dynamic memory
        free(params->conf_fname) ;
        free(params->file_backend_name) ;
        free(params->dbm_backend_name) ;
        free(params->directory_backend_name) ;
        free(params->thread_launch_name) ;
        free(params->mfs_server_stub_pname) ;
        free(params->ns_backend_name) ;

      	// return OK
      	return 1;
}

