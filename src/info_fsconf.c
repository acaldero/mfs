

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

 #include "info_fsconf.h"

 int info_fsconf_add_partition ( conf_t *conf )
 {
	int ret, part_in_bytes ;

	(conf->n_partitions)++ ;

	part_in_bytes = (conf->n_partitions) * sizeof(conf_part_t) ;
	ret = mfs_realloc((char **)&(conf->partitions), part_in_bytes) ;
	if (ret < 0) {
	    return -1 ;
	}

      	// return OK
      	return 1 ;
 }

 int info_fsconf_add_node ( conf_t *conf, int id_part )
 {
	int ret, nodes_in_bytes ;

        (conf->partitions[id_part].n_nodes)++ ;

	nodes_in_bytes = (conf->partitions[id_part].n_nodes) * sizeof(char *) ;
	ret = mfs_realloc((char **)&(conf->partitions[id_part].nodes), nodes_in_bytes) ;
	if (ret < 0) {
	    return -1 ;
	}

	nodes_in_bytes = (conf->partitions[id_part].n_nodes) * sizeof(base_url_t) ;
	ret = mfs_realloc((char **)&(conf->partitions[id_part].url), nodes_in_bytes) ;
	if (ret < 0) {
	    return -1 ;
	}

      	// return OK
      	return 1 ;
 }


 //
 // API
 //

 int info_fsconf_show ( conf_t *conf )
 {
 	printf(" Current configuration:\n");
	for (int i=0; i<conf->n_partitions; i++)
	{
 	     printf(" | - partition:\n");
             printf(" |   | - name: '%s'\n", conf->partitions[i].name) ;
             printf(" |   | - type: '%s'\n", conf->partitions[i].type) ;
             printf(" |   | - nodes:\n") ;
	     for (int j=0; j<conf->partitions[i].n_nodes; j++) {
                  printf(" |   |   | - uri: '%s'\n", conf->partitions[i].nodes[j]) ;
	     }
	}

      	// return OK
      	return 1 ;
 }

 int info_fsconf_get ( conf_t *conf, char *yaml_file_name )
 {
	int ret ;
        FILE *fh ;
	int  in_loop, kv_type, k_type ;
        yaml_parser_t parser ;
	yaml_token_t  token ;
	char *token_val ;
	int   id_last_part, id_last_node ;

        /* Initialize conf */
	conf->n_partitions = 0 ;
	conf->partitions   = NULL ;
	conf->active       = NULL ;

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
			 break ;
		    case YAML_STREAM_END_TOKEN:
			 break ;

		    /* Token types (read before actual token) */
		    case YAML_KEY_TOKEN:
	                 kv_type = 1 ;
			 break ;
		    case YAML_VALUE_TOKEN:
	                 kv_type = 2 ;
			 break ;

		    /* Block delimeters */
		    case YAML_BLOCK_SEQUENCE_START_TOKEN:
			 break ;
		    case YAML_BLOCK_ENTRY_TOKEN:
			 break ;
		    case YAML_BLOCK_END_TOKEN:
			 break ;

		    /* Data */
		    case YAML_BLOCK_MAPPING_START_TOKEN:
			 break ;
		    case YAML_SCALAR_TOKEN:
	                 token_val    = (char *)token.data.scalar.value ;
			 id_last_part = conf->n_partitions - 1 ;

	                 if (1 == kv_type)
			 {
			     if (!strcmp("partition", token_val)) {
                                 ret = info_fsconf_add_partition(conf) ;
				 if (ret < 0) return -1 ;
			     }
			     if (!strcmp("name",   token_val))  k_type = 1 ;
			     if (!strcmp("type",   token_val))  k_type = 2 ;
			     if (!strcmp("nodes",  token_val))  k_type = 3 ;
			     if (!strcmp("uri",    token_val))  k_type = 4 ;
			 }
	                 if (2 == kv_type)
			 {
			     if (1 == k_type) {
                                 conf->partitions[id_last_part].name = strdup(token_val) ;
			     }
			     if (2 == k_type) {
                                 conf->partitions[id_last_part].type = strdup(token_val) ;
			     }
			     if (4 == k_type)
			     {
				 ret = info_fsconf_add_node(conf, id_last_part) ;
				 if (ret < 0) return -1 ;

				 id_last_node = conf->partitions[id_last_part].n_nodes - 1 ;
				 conf->partitions[id_last_part].nodes[id_last_node] = strdup(token_val) ;

				 ret = base_url_parseURL(&(conf->partitions[id_last_part].url[id_last_node]), token_val) ;
				 if (ret < 0) return -1 ;
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

	// active
	if (conf->n_partitions > 0) {
	    conf->active = &(conf->partitions[0]) ;
	}

      	// return OK
      	return 1 ;
 }

 int info_fsconf_free ( conf_t *conf )
 {
	// Check arguments...
	if (NULL == conf) {
	    return -1 ;
	}
	if (0 == conf->n_partitions) {
      	    return 1 ;
	}

	// Free each partition...
	for (int i=0; i<conf->n_partitions; i++)
	{
	     // free name and type
             free(conf->partitions[i].name) ;
             free(conf->partitions[i].type) ;

	     // free each node...
	     for (int j=0; j<conf->partitions[i].n_nodes; j++)
	     {
                  free(conf->partitions[i].nodes[j]) ;
                  base_url_freeURL(&(conf->partitions[i].url[j])) ;
	     }

	     // ...and node tables
             free(conf->partitions[i].nodes) ;
             free(conf->partitions[i].url) ;
	}
	// ...and partition table
        free(conf->partitions) ;

        /* Initialize conf */
	conf->n_partitions = 0 ;
	conf->partitions   = NULL ;
	conf->active       = NULL ;

      	// return OK
      	return 1 ;
 }


 //
 // Getters
 //

 int   info_fsconf_get_active_nnodes ( conf_t *conf )
 {
	if (NULL == conf) {
	    return -1 ;
	}
	if (NULL == conf->active) {
	    return -1 ;
	}

      	// return number of nodes
	return (conf->active->n_nodes) ;
 }

 char *info_fsconf_get_active_node ( conf_t *conf, int rank )
 {
	if (NULL == conf) {
	    return NULL ;
	}

        if (rank >= conf->active->n_nodes) {
	    return NULL ;
	}

      	// return node (char *)
        return conf->active->nodes[rank]  ;
 }

 base_url_t *info_fsconf_get_active_url ( conf_t *conf, int rank )
 {
	if (NULL == conf) {
	    return NULL ;
	}

        if (rank >= conf->active->n_nodes) {
	    return NULL ;
	}

      	// return node (char *)
        return &(conf->active->url[rank])  ;
 }

