

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

 #include "mfs_conf.h"

 int mfs_conf_show ( conf_t *conf )
 {
 	printf("Current configuration:\n");
	for (int i=0; i<conf->n_partitions; i++)
	{
 	     printf("\tpartition:\n");
             printf("\t\tname: '%s'\n", conf->partitions[i].name) ;
             printf("\t\ttype: '%s'\n", conf->partitions[i].type) ;
             printf("\t\tnodes:\n") ;
	     for (int j=0; j<conf->partitions[i].n_nodes; j++)
                  printf("\t\t\t- uri: '%s'\n", conf->partitions[i].nodes[j]) ;
	}

      	// return OK
      	return 1 ;
 }

 int mfs_conf_get ( conf_t *conf, char *yaml_file_name )
 {
        FILE *fh ;
	int  in_loop, kv_type, k_type ;
        yaml_parser_t parser ;
	yaml_token_t  token ;
	char *token_val ;
	int   id_last_part ;

        /* Initialize conf */
	conf->n_partitions = 0 ;
	conf->partitions   = NULL ;

        /* Initialize parser */
        if (!yaml_parser_initialize(&parser)) {
	    mfs_print(DBG_ERROR, "[YAML] Failed to initialize parser!\n") ;
	    return -1 ;
        }

        /* Open file */
        fh = fopen(yaml_file_name, "r") ;
        if (fh == NULL) {
	   mfs_print(DBG_ERROR, "[YAML] Failed to open file '%s'!\n", yaml_file_name) ;
           yaml_parser_delete(&parser) ;
	   return -1 ;
        }

        /* Set input file */
        yaml_parser_set_input_file(&parser, fh) ;

        /* Read tokens */
	in_loop = 1 ;
	kv_type = 0 ;
	k_type  = 0 ;
	while (in_loop)
	{
	    yaml_parser_scan(&parser, &token) ;
	    switch (token.type)
	    {
		    /* Stream start/end */
		    case YAML_STREAM_START_TOKEN:
			 //printf("STREAM START") ;
			 break ;
		    case YAML_STREAM_END_TOKEN:
			 //printf("STREAM END") ;
			 break ;

		    /* Token types (read before actual token) */
		    case YAML_KEY_TOKEN:
			 //printf("(Key token)   ") ;
	                 kv_type = 1 ;
			 break ;
		    case YAML_VALUE_TOKEN:
			 //printf("(Value token) ") ;
	                 kv_type = 2 ;
			 break ;

		    /* Block delimeters */
		    case YAML_BLOCK_SEQUENCE_START_TOKEN:
			 //printf("<b>Start Block (Sequence)</b>") ;
			 break ;
		    case YAML_BLOCK_ENTRY_TOKEN:
			 //printf("<b>Start Block (Entry)</b>") ;
			 break ;
		    case YAML_BLOCK_END_TOKEN:
			 //printf("<b>End block</b>") ;
			 break ;

		    /* Data */
		    case YAML_BLOCK_MAPPING_START_TOKEN:
			 //printf("[Block mapping]") ;
			 break ;
		    case YAML_SCALAR_TOKEN:
			 //printf("scalar %s \n", token.data.scalar.value) ;
	                 token_val    = (char *)token.data.scalar.value ;
			 id_last_part = conf->n_partitions - 1 ;

	                 if (1 == kv_type)
			 {
			     if (!strcmp("partition",  token_val)) {
				                     (conf->n_partitions)++ ;
				 int part_in_bytes = (conf->n_partitions) * sizeof(conf_part_t) ;
				 conf->partitions  = (conf_part_t *)realloc(conf->partitions, part_in_bytes) ;
			     }
			     if (!strcmp("name",       token_val))  k_type = 1 ;
			     if (!strcmp("type",       token_val))  k_type = 2 ;
			     if (!strcmp("nodes",      token_val))  k_type = 3 ;
			     if (!strcmp("uri",        token_val))  k_type = 4 ;
			 }
	                 if (2 == kv_type)
			 {
			     if (1 == k_type) {
                                 conf->partitions[id_last_part].name = strdup(token_val) ;
			     }
			     if (2 == k_type) {
                                 conf->partitions[id_last_part].type = strdup(token_val) ;
			     }
			     if (4 == k_type) {
	                         int id_last_node   =  conf->partitions[id_last_part].n_nodes ;
	                                              (conf->partitions[id_last_part].n_nodes)++ ;
				 int nodes_in_bytes = (conf->partitions[id_last_part].n_nodes) * sizeof(char *) ;
				  conf->partitions[id_last_part].nodes               = (char **)realloc(conf->partitions[id_last_part].nodes, nodes_in_bytes) ;
                                  conf->partitions[id_last_part].nodes[id_last_node] = strdup(token_val) ;
			     }
			 }
			 break ;

		    /* Others */
		    default:
			 //printf("Got token of type %d\n", token.type) ;
			 break ;
	    }
	    if (YAML_STREAM_END_TOKEN == token.type) {
	        in_loop = 0 ;
	    }
	    yaml_token_delete(&token) ;
	}

        /* Cleanup */
        yaml_parser_delete(&parser) ;
        fclose(fh) ;

      	// return OK
      	return 1 ;
 }

 int mfs_conf_free ( conf_t *conf )
 {
	// free each partition...
	for (int i=0; i<conf->n_partitions; i++)
	{
	     // free name and type
             free(conf->partitions[i].name) ;
             free(conf->partitions[i].type) ;

	     // free each node... 
	     for (int j=0; j<conf->partitions[i].n_nodes; j++) {
                  free(conf->partitions[i].nodes[j]) ;
	     }
	     // ...and node table
             free(conf->partitions[i].nodes) ;
	}
	// ...and partition table
        free(conf->partitions) ;

      	// return OK
      	return 1 ;
 }

