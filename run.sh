#!/bin/sh
set -x

echo "./mfs_server &"
mpirun -np 1 valgrind ./mfs_server &
sleep 2


for i in $(seq 1 1 3)
do
   echo "./mfs_client...($i)"
   mpirun -np 1 valgrind ./mfs_client
   sleep 1
done


