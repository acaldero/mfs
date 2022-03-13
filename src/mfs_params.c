

/*
 *  Copyright 2020-2022 Felix Garcia Carballeira, Diego Camarmas Alonso, Alejandro Calderon Mateos
 *
 *  This file is part of MPI_PFS.
 *
 *  MPI_PFS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MPI_PFS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MPI_PFS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

 #include "mfs_params.h"

 void mfs_params_show ( params_t *params )
 {
 	printf("Current configuration:\n");
      	printf("\t-d <base directory>:\t\t'%s'\n",     params->data_prefix) ;
      	printf("\t-f <POSIX | MPI-IO>:\t\t%s\n",       params->file_backend_name) ;
      	printf("\t-i <POSIX>:\t\t\t%s\n",              params->directory_backend_name) ;
      	printf("\t-b <GDBM>:\t\t\t%s\n",               params->dbm_backend_name) ;
      	printf("\t-t <ondemand | pool>:\t\t'%s'\n",    params->thread_launch_name) ;
      	printf("\t-n <# process in server>:\t'%d'\n",  params->num_servers) ;
 }

 void mfs_params_show_usage ( void )
 {
      	printf("Usage:\n");
      	printf("\t-d <string>:  name of the base directory\n") ;
      	printf("\t-f <string>:  POSIX | MPI-IO\n") ;
      	printf("\t-i <string>:  POSIX\n") ;
      	printf("\t-b <string>:  GDBM\n") ;
      	printf("\t-t <string>:  ondemand | pool\n") ;
      	printf("\t-n <integer>: number of servers\n") ;
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

        params->num_servers   = 1 ;
        strcpy(params->data_prefix, DEFAULT_DATA_PREFIX) ;

        params->file_backend = FILE_USE_POSIX ;
        strcpy(params->file_backend_name, "POSIX") ;

        params->dbm_backend  = DBM_USE_GDBM ;
        strcpy(params->dbm_backend_name, "GDBM") ;

        params->directory_backend = DIRECTORY_USE_POSIX ;
        strcpy(params->directory_backend_name, "POSIX") ;

        params->thread_launch = THREAD_USE_ONDEMAND ;
        strcpy(params->thread_launch_name, "On demand") ;

        params->comm_backend = COMM_USE_MPI ;
        strcpy(params->comm_backend_name, "MPI") ;

        strcpy(params->mfs_server_stub_pname, DEFAULT_STUB_PNAME) ;

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
		  strcpy(params->data_prefix, optarg) ;
                  break ;

	     case 'n':
		  params->num_servers = atoi(optarg) ;
                  break ;

	     case 'f':
		  if (!strcmp("POSIX",  optarg)) {
		      params->file_backend = FILE_USE_POSIX ;
		      strcpy(params->file_backend_name, "POSIX") ;
		  }
		  if (!strcmp("MPI-IO", optarg)) {
		      params->file_backend = FILE_USE_MPI_IO ;
		      strcpy(params->file_backend_name, "MPI-IO") ;
		  }
                  break ;

	     case 'b':
		  if (!strcmp("GDBM",  optarg)) {
		      params->dbm_backend = DBM_USE_GDBM ;
		      strcpy(params->dbm_backend_name, "GDBM") ;
		  }
		  if (!strcmp("REDIS", optarg)) {
		      params->dbm_backend = DBM_USE_REDIS ;
		      strcpy(params->dbm_backend_name, "REDIS") ;
		  }
                  break ;

	     case 'i':
		  if (!strcmp("POSIX",  optarg)) {
		      params->directory_backend = DIRECTORY_USE_POSIX ;
		      strcpy(params->directory_backend_name, "POSIX") ;
		  }
		  if (!strcmp("REDIS", optarg)) {
		      params->directory_backend = DIRECTORY_USE_REDIS ;
		      strcpy(params->directory_backend_name, "REDIS") ;
		  }
                  break ;

	     case 't':
		  if (!strcmp("ondemand", optarg) ) {
		      params->thread_launch = THREAD_USE_ONDEMAND ;
		      strcpy(params->thread_launch_name, "On Demand") ;
		  }
		  if (!strcmp("pool",     optarg) ) {
		      params->thread_launch = THREAD_USE_POOL ;
		      strcpy(params->thread_launch_name, "Pool") ;
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

