#!/bin/sh
#set -x

#
# initialization
# 
HOSTNAME=$(hostname)
SERVER_NP=2
CLIENT_NP=2
N_TESTS=2

# just in case, create data directory
mkdir -p ./data

# https://stackoverflow.com/questions/360201/how-do-i-kill-background-processes-jobs-when-my-shell-script-exits
trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

#
# start namespace server...
#
echo "............................."
echo "hydra_nameserver &"
echo "sleep 1"
echo "............................."
hydra_nameserver &
sleep 1

#
# start server...
#
echo "............................."
echo "./mfs_server &"
echo "sleep 3"
echo "............................."
mpirun -np $SERVER_NP -nameserver ${HOSTNAME} ./mfs_server &
sleep 3

#
# run client...
#
for i in $(seq 1 1 $N_TESTS)
do
   echo "............................."
   echo "./mfs_client...(test $i)"
   echo "sleep 2"
   echo "............................."
   mpirun -np $CLIENT_NP -nameserver ${HOSTNAME} ./mfs_client
   echo "............................."
   sleep 2
done

#
# stop server...
#
echo "sleep 1"
echo "kill <mfs_server>"
sleep 1
kill -- -$$

