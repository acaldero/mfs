#!/bin/sh
set -x

mpirun -np 1 ./mfs_server &
sleep 1
mpirun -np 2 ./mfs_client

