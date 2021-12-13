#!/bin/sh
#set -x

# https://stackoverflow.com/questions/360201/how-do-i-kill-background-processes-jobs-when-my-shell-script-exits
trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

# start namespace server...
##hydra_nameserver &

# start server...
echo "............................."
echo "./mfs_server &"
echo "sleep 3"
echo "............................."
mpirun -np 2 -nameserver ${HOSTNAME} ./mfs_server &
sleep 3

# run clients...
for i in $(seq 1 1 2)
do
   echo "............................."
   echo "./mfs_client...(test $i)"
   echo "sleep 2"
   echo "............................."
   mpirun -np 2 -nameserver ${HOSTNAME} ./mfs_client
   echo "............................."
   sleep 2
done

# stop server...
echo "sleep 1"
echo "kill <mfs_server>"
sleep 1
kill -- -$$

