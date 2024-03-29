# Project
cmake_minimum_required (VERSION 3.20.0)

project (XPNlite)

SET    (CMAKE_BUILD_TYPE distribution)
SET    (CMAKE_C_COMPILER "mpic++")

SET    (CMAKE_C_FLAGS_DISTRIBUTION "-w -g -O2 -Wall")
string (APPEND CMAKE_C_FLAGS_DISTRIBUTION " -I/usr/local/include/ -I./  -I./include ")
string (APPEND CMAKE_C_FLAGS_DISTRIBUTION " -I/usr/local/include/ -I../ -I../include ")
string (APPEND CMAKE_C_FLAGS_DISTRIBUTION " -DHAVE_GDBM_H -DHAVE_HIREDIS_H -DHAVE_TDB_H ")
SET    (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "lib")
SET    (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "bin")

# source 
SET    (MFS_SRC src/mfs_api.c
	src/client_stub_mpi.c     src/client_stub_socket.c    src/client_stub_local.c
        src/server_stub_mpi.c     src/server_stub_socket.c
	src/mfs_comm.c            src/mfs_comm_mpi.c          src/mfs_comm_socket.c
	src/mfs_files.c           src/mfs_files_posix.c       src/mfs_files_mpi.c
	src/mfs_directories.c     src/mfs_directories_posix.c
        src/mfs_dbm.c             src/mfs_dbm_gdbm.c          src/mfs_dbm_redis.c          src/mfs_dbm_tdb.c
	src/mfs_workers.c         src/mfs_workers_pool.c      src/mfs_workers_onrequest.c
	src/info_ns.c             src/info_params.c           src/info_fsconf.c
	src/mfs_descriptors.c     src/base_socket.c           src/base_url.c
	src/base_lib.c)

# Compiles the sources only once
add_library(MFS_OBJ OBJECT ${MFS_SRC})

# shared libraries need PIC
set_property(TARGET MFS_OBJ PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(mfs_client.so SHARED $<TARGET_OBJECTS:MFS_OBJ>)
add_library(mfs_client.a  STATIC $<TARGET_OBJECTS:MFS_OBJ>)

add_executable        (mfs_server_mpi    src/server_mpi.c)
target_link_libraries (mfs_server_mpi LINK_PUBLIC -lpthread -lhiredis -lgdbm -lyaml -ltdb  mfs_client.a)

add_executable        (mfs_server_socket src/server_socket.c)
target_link_libraries (mfs_server_socket LINK_PUBLIC -lpthread -lhiredis -lgdbm -lyaml -ltdb  mfs_client.a)


