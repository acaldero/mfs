# MPI_PFS (v0.92)
Prototype of MPI_PFS

## Getting started

### Install prerequisites

```
sudo apt-get install -y build-essential cmake
sudo apt-get install -y flex libtool
sudo apt-get install -y mpich mpich-doc
sudo apt-get install -y libhiredis-dev
```

### Get MPI_PFS source code

```
git clone https://github.com/acaldero/mpi_pfs.git
cd mpi_pfs
```

### Compile MPI_PFS

```
make         -f Makefile_make
make -C test -f Makefile_make
```

## Executing MPI_PFS

### Example of work session

<html>
 <table>
  <tr>
  <th>Server</th>
  <th>Client</th>
  </tr>
  <tr>
  <td>
  cd test<br>
  HYDRA_HOST=$(hostname)<br>
  hydra_nameserver & <br>
  mpirun -nameserver ${HYDRA_HOST} -np 2 ../bin/mfs_server &<br>
  </td>
  <td>
  &nbsp;
  </td>
  </tr>
  <tr>
  <td>
  &nbsp;
  </td>
  <td>
  cd test<br>
  mpirun -nameserver ${HYDRA_HOST} -np 2 ./test_benchmark
  </td>
  </tr>
  </table>
</html>


## About

This project is part of the ADMIRE project:
https://www.admire-eurohpc.eu/

GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
Felix Garcia Carballeira + Diego Camarmas Alonso + Alejandro Calderon Mateos

