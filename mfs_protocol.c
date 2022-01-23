
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


#include "mfs_protocol.h"


//
// Send request "header"
//

int mfs_protocol_request_send ( comm_t *cb, int rank, msg_t *msg )
{
    int ret ;

    // Send msg
    ret = mfs_comm_send_data_to(cb, rank, msg, 3, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[PROTOCOL]: MPI_Send fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int mfs_protocol_request_receive ( comm_t *cb, msg_t *msg )
{
    int ret ;

    // Receive msg
    ret = mfs_comm_recv_data_from(cb, MPI_ANY_SOURCE, msg, 3, MPI_INT) ;
    if (ret < 0) {
        mfs_print(DBG_ERROR, "[PROTOCOL]: MPI_Recv fails :-(") ;
        return -1 ;
    }

    // Return OK/KO
    return ret ;
}

int mfs_protocol_request_do ( comm_t *wb, buffer_t *info, int neltos )
{
    int ret ;

    ret = 1 ;
    for (int i=0; i<neltos; i++)
    {
         switch (info[i].comm_action)
         {
             case COM_SEND_DATA_TO:
                  ret =   mfs_comm_send_data_to(wb, info[i].remote, info[i].buff, info[i].size, info[i].datatype) ;
                  break;
             case COM_RECV_DATA_FROM:
	          ret = mfs_comm_recv_data_from(wb, info[i].remote, info[i].buff, info[i].size, info[i].datatype) ;
                  break;

             case COM_MALLOC:
                  ret = mfs_malloc((char **)(info[i].buff), info[i].size) ;
                  break;
             case COM_FREE:
                  ret =   mfs_free((char **)(info[i].buff)) ;
                  break;

             case COM_SEND_PTRDATA_TO:
                  ret =   mfs_comm_send_data_to(wb, info[i].remote, *((void **)(info[i].buff)), info[i].size, info[i].datatype) ;
                  break;
             case COM_RECV_PTRDATA_FROM:
	          ret = mfs_comm_recv_data_from(wb, info[i].remote, *((void **)(info[i].buff)), info[i].size, info[i].datatype) ;
                  break;

             case COM_FILE_OPEN:
                  break;
             case COM_FILE_CLOSE:
                  break;
             case COM_FILE_WRITE:
                  info[i].size = server_files_write(info[i].remote, info[i].buff, info[i].size) ;
                  break;
             case COM_FILE_READ:
                  info[i].size =  server_files_read(info[i].remote, info[i].buff, info[i].size) ;
                  break;
             default:
                  break;
         }

	 if (ret < 0) {
	     mfs_print(DBG_ERROR, info[i].err_msg, wb->rank) ;
	     return -1 ;
	 }
    }

    // Return OK/KO
    return ret ;
}

