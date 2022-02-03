

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
      	printf("\t-p <POSIX|MPI-IO>:\t\t%s\n",          params->file_protocol_name) ;
      	printf("\t-d <base directory>:\t\t'%s'\n",      params->data_prefix) ;
      	printf("\t-n <# process in server>:\t'%d'\n",   params->num_servers) ;
 }
      
 void mfs_params_show_usage ( void )
 {
      	printf("Usage:\n");
      	printf("\t-p <string>:  POSIX | MPI-IO\n") ;
      	printf("\t-d <string>:  name of the base directory\n") ;
      	printf("\t-n <integer>: number of servers\n") ;
 }
      
 int mfs_params_get ( params_t *params, int *argc, char ***argv )
 {
      	// set default values
      	params->argc = argc ;
      	params->argv = argv ;

        params->num_servers   = 1 ;
        params->file_protocol = FILE_USE_POSIX ;
        strcpy(params->file_protocol_name, "POSIX") ;
        strcpy(params->data_prefix, DEFAULT_DATA_PREFIX) ;

      	// update user requests
      	for (int i=0; i<(*argc); i++)
      	{
      		switch ((*argv)[i][0])
      		{
      			case '-':
      				switch ((*argv)[i][1])
				{
      					case 'p':
						if (!strcmp("POSIX",  (*argv)[i+1]) ) {
      						    params->file_protocol = FILE_USE_POSIX ;
                                                    strcpy(params->file_protocol_name, "POSIX") ;
						}
						if (!strcmp("MPI-IO", (*argv)[i+1]) ) {
      						    params->file_protocol = FILE_USE_MPI_IO ;
                                                    strcpy(params->file_protocol_name, "MPI-IO") ;
						}
      						i++;
      						break;

      					case 'd':
      						strcpy(params->data_prefix, (*argv)[i+1]) ;
      						i++;
      						break;					

      					case 'n':
      						params->num_servers = atoi((*argv)[i+1]) ;
      						i++;
      						break;					

      					case 'h':
      						return -1;
      
      					default:
      						break;
      				}
      				break;

      			default:
      				break;			
      		}
      	}

      	// return OK
      	return 1;
 }

