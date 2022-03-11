

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
      	printf("\t-p <POSIX | MPI-IO>:\t\t%s\n",       params->file_protocol_name) ;
      	printf("\t-d <base directory>:\t\t'%s'\n",     params->data_prefix) ;
      	printf("\t-t <ondemand | pool>:\t\t'%s'\n",    params->thread_launch_name) ;
      	printf("\t-n <# process in server>:\t'%d'\n",  params->num_servers) ;
 }

 void mfs_params_show_usage ( void )
 {
      	printf("Usage:\n");
      	printf("\t-p <string>:  POSIX | MPI-IO\n") ;
      	printf("\t-d <string>:  name of the base directory\n") ;
      	printf("\t-t <string>:  ondemand | pool\n") ;
      	printf("\t-n <integer>: number of servers\n") ;
 }

 struct option long_options[] =
        {
          { "verbose",            no_argument,       NULL, 'v' },
          { "file_backend",       required_argument, NULL, 'p' },
          { "base_directory",     required_argument, NULL, 'd' },
          { "thread_backend",     required_argument, NULL, 't' },
          { "process_in_server",  required_argument, NULL, 'n' },
          { 0 }
        } ;

 int mfs_params_get ( params_t *params, int *argc, char ***argv )
 {
        int c ;
	int option_index = 0;

      	// set default values
      	params->argc = argc ;
      	params->argv = argv ;

        params->num_servers   = 1 ;
        strcpy(params->data_prefix, DEFAULT_DATA_PREFIX) ;

        params->file_protocol = FILE_USE_POSIX ;
        strcpy(params->file_protocol_name, "POSIX") ;

        params->thread_launch = THREAD_USE_ONDEMAND ;
        strcpy(params->thread_launch_name, "On demand") ;

	// getopt_long...
        c = getopt_long(*argc, *argv, "vp:d:t:n:", long_options, &option_index) ;
	while (c != -1)
	{
           switch (c)
           {
             case 'v':
		  // verbose
                  break ;

	     case 'p':
		  if (!strcmp("POSIX",  optarg)) {
		      params->file_protocol = FILE_USE_POSIX ;
		      strcpy(params->file_protocol_name, "POSIX") ;
		  }
		  if (!strcmp("MPI-IO", optarg)) {
		      params->file_protocol = FILE_USE_MPI_IO ;
		      strcpy(params->file_protocol_name, "MPI-IO") ;
		  }
                  break ;

	     case 'd':
		  strcpy(params->data_prefix, optarg) ;
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

	     case 'n':
		  params->num_servers = atoi(optarg) ;
                  break ;

             case '?':
                  mfs_params_show_usage() ;
                  exit(-1) ;
                  break ;

             default:
                  mfs_params_show_usage() ;
                  exit(-1) ;
           }

           c = getopt_long(*argc, *argv, "vp:d:t:n:", long_options, &option_index) ;
	}

      	// return OK
      	return 1;
 }

