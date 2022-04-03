# Project
cmake_minimum_required (VERSION 2.8.11)
project (XPNlite)

SET (CMAKE_BUILD_TYPE distribution)
SET (CMAKE_C_FLAGS_DISTRIBUTION "-w -g -O2 -Wall -I/usr/local/include/ -I./ -DDEBUG")
SET (CMAKE_C_COMPILER "mpic++")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "bin")


# Executable 
ADD_LIBRARY (mfs_client.a STATIC
             src/mfs_api.c 
	     src/client_stub_mpi.c     src/client_stub_socket.c    src/client_stub_local.c
             src/server_stub_mpi.c     src/server_stub_socket.c
	     src/mfs_comm.c            src/mfs_comm_mpi.c          src/mfs_comm_socket.c 
	     src/mfs_files.c           src/mfs_files_posix.c       src/mfs_files_mpi.c
	     src/mfs_directories.c     src/mfs_directories_posix.c
             src/mfs_dbm.c             src/mfs_dbm_gdbm.c          src/mfs_dbm_redis.c
	     src/mfs_workers.c         src/mfs_workers_pool.c      src/mfs_workers_onrequest.c
	     src/info_ns.c             src/info_params.c           src/info_fsconf.c
	     src/mfs_descriptors.c     src/base_socket.c           src/base_url.c
	     src/base_lib.c)
add_executable (mfs_server_mpi    src/mfs_server_mpi.c)
add_executable (mfs_server_socket src/mfs_server_socket.c)


# Link the executable 
target_link_libraries (mfs_server_mpi LINK_PUBLIC -lpthread  -lhiredis  -lgdbm  -lyaml  mfs_client.a)

