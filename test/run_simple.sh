#!/bin/sh
#set -x

#
# initialization
# 
HOSTNAME=$(hostname)
SERVER_NP=2
CLIENT_NP=1
N_TESTS=1
F_BACKEND=POSIX
V_LEVEL=1


#
# Start SIMPLE
#
echo ""
echo " SIMPLE"
echo " ~~~~~~"
# just in case, create data directory
mkdir -p ./data
# https://stackoverflow.com/questions/360201/how-do-i-kill-background-processes-jobs-when-my-shell-script-exits
trap "trap - TERM && kill -- -$$" INT TERM EXIT


#
# MPI tests...
#
if true; then
   echo ""
   echo " # comm_backend=MPI ##################################### "
   echo ""
   # start namespace server...
   echo "   + hydra_nameserver &"
   echo "   + sleep 1"
     hydra_nameserver &
     sleep 1
   # run server...
   echo "   + ../bin/mfs_server_mpi &"
   echo "   + sleep 3"
     mpirun -np $SERVER_NP -nameserver ${HOSTNAME} ../bin/mfs_server_mpi    -f ${F_BACKEND} -c MPI -v ${V_LEVEL} &
     sleep 3
   # run client...
   echo ""
   for i in $(seq 1 1 $N_TESTS)
   do
      echo "   + ./test_simple ...(test $i)"
      echo "   + sleep 2"
        mpirun -np $CLIENT_NP -nameserver ${HOSTNAME} ./test_simple -n conf.yaml -f ${F_BACKEND} -c MPI -v ${V_LEVEL}
        sleep 2
   done
fi


#
# Sockets tests...
#
if false; then
   echo ""
   echo " # comm_backend=SOCKET ################################## "
   echo ""
   # start server...
   echo "   + ../bin/mfs_server &"
   echo "   + sleep 3"
    mpirun -np $SERVER_NP -nameserver ${HOSTNAME} ../bin/mfs_server_socket -f ${F_BACKEND} -c SOCKET -v ${V_LEVEL} &
    sleep 3
   # run client...
   echo ""
   for i in $(seq 1 1 $N_TESTS)
   do
      echo "   + ./test_simple ...(test $i)"
      echo "   + sleep 2"
       mpirun -np $CLIENT_NP -nameserver ${HOSTNAME} ./test_simple -n conf.yaml -f ${F_BACKEND} -c SOCKET -p 0 -v ${V_LEVEL}
       sleep 2
   done
fi


#
# Local tests...
#
if true; then
   echo ""
   echo " # comm_backend=LOCAL ################################### "
   echo ""
   # run client...
   for i in $(seq 1 1 $N_TESTS)
   do
      echo "   + ./test_simple ...(test $i)"
      echo "   + sleep 2"
      mpirun -np $CLIENT_NP -nameserver ${HOSTNAME} ./test_simple -n conf.yaml -f ${F_BACKEND} -c LOCAL -v ${V_LEVEL}
      sleep 2
   done
fi


#
# Stop SIMPLE
#
echo ""
echo " ######################################################## "
echo ""
echo "   + sleep 1"
echo "   + kill <mfs_server>"
  sleep 1
  kill -- -$$

