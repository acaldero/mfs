#!/bin/sh
#set -x

#
# initialization
# 
HOSTNAME=$(hostname)
SERVER_NP=1
CLIENT_NP=1
N_TESTS=1
F_BACKEND=POSIX

# just in case, create data directory
mkdir -p ./data

# https://stackoverflow.com/questions/360201/how-do-i-kill-background-processes-jobs-when-my-shell-script-exits
trap "trap - TERM && kill -- -$$" INT TERM EXIT

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
echo "../bin/mfs_server &"
echo "sleep 3"
echo "............................."
 mpirun -np $SERVER_NP -nameserver ${HOSTNAME} ../bin/mfs_server_mpi    -f ${F_BACKEND} -c MPI &
#mpirun -np $SERVER_NP -nameserver ${HOSTNAME} ../bin/mfs_server_socket -f ${F_BACKEND} -c SOCKET &
sleep 3

#
# run client...
#
for i in $(seq 1 1 $N_TESTS)
do
   echo "............................."
   echo "./test_simple ...(test $i)"
   echo "sleep 2"
   echo "............................."
   mpirun -np $CLIENT_NP -nameserver ${HOSTNAME} ./test_simple -n conf.yaml -f ${F_BACKEND} -c MPI
  #mpirun -np $CLIENT_NP -nameserver ${HOSTNAME} ./test_simple -n conf.yaml -f ${F_BACKEND} -c SOCKET -p 0
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

