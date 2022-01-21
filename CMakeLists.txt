# Project
cmake_minimum_required (VERSION 2.8.11)
project (MPI_PFS)

SET (CMAKE_BUILD_TYPE distribution)
SET (CMAKE_C_FLAGS_DISTRIBUTION "-w -g -O2 -Wall -I/usr/local/include/ -I./ -DDEBUG")
SET (CMAKE_C_COMPILER "mpic++")

# Executable 
add_executable (mfs_server mfs_server.c mfs_server_stub.c mfs_files.c mfs_comm.o mfs_lib.c)
add_executable (mfs_client mfs_client.c mfs_client_stub.c             mfs_comm.o mfs_lib.c)

# Link the executable 
target_link_libraries (mfs_server LINK_PUBLIC -lpthread)

