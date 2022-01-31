# Project
cmake_minimum_required (VERSION 2.8.11)
project (MPI_PFS)

SET (CMAKE_BUILD_TYPE distribution)
SET (CMAKE_C_FLAGS_DISTRIBUTION "-w -g -O2 -Wall -I/usr/local/include/ -I./ -DDEBUG")
SET (CMAKE_C_COMPILER "mpic++")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "bin")

# Executable 
ADD_LIBRARY (mfs_client.a STATIC
             src/mfs_server_stub.c src/mfs_client_stub.c src/mfs_workers.c src/mfs_protocol.c src/mfs_comm.c src/mfs_files.c src/mfs_params.c src/mfs_directories.c src/mfs_lib.c)
add_executable (mfs_server src/mfs_server.c)

# Link the executable 
target_link_libraries (mfs_server LINK_PUBLIC -lpthread mfs_client.a)

