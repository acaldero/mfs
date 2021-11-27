#!/bin/sh
#set -x

# https://stackoverflow.com/questions/360201/how-do-i-kill-background-processes-jobs-when-my-shell-script-exits
trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

# start server...
echo "............................."
echo "./mfs_server &"
echo "............................."
mpirun -np 1 ./mfs_server &
echo "sleep 3"
sleep 3

# run clients...
for i in $(seq 1 1 3)
do
   echo "............................."
   echo "./mfs_client...($i)"
   echo "............................."
   mpirun -np 1 ./mfs_client
   echo "............................."
   echo "sleep 1"
   sleep 1
done

# stop server...
kill -- -$$

