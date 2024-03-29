# XPNLite (v1.00)
XPNLite: compact version of the XPN Parallel File System for prototyping

*Licence*: GNU LESSER GENERAL PUBLIC LICENSE Version 2.1</br>
*Authors*: [Felix Garcia Carballeira](https://researchportal.uc3m.es/display/inv16287) + [Diego Camarmas Alonso](https://github.com/dcamarmas/) + [Alejandro Calderon Mateos](https://github.com/acaldero)</br>
*Context*: XPNLite is related to the European ADMIRE project: https://www.admire-eurohpc.eu/


## Getting started

### Install prerequisites

#### Mandatory (developer + MPICH + yaml + tdb)

```
sudo apt-get install -y build-essential cmake flex libtool
sudo apt-get install -y mpich mpich-doc
sudo apt-get install -y libyaml-dev libyaml-doc
sudo apt-get install -y libtdb-dev tdb-tools
```

#### Optionals (GDBM + REDIS)
```
sudo apt-get install -y libgdbm-compat-dev libgdbm-dev
sudo apt-get install -y libhiredis-dev
```


### Get XPNLite source code

```
git clone https://github.com/acaldero/mfs.git
cd mfs
```

### Compile XPNLite

```
make -f Makefile_make
make -f Makefile_make -C test
```


## Executing XPNLite

### Examples included in XPNLite

```
  cd test
  ./run_simple.sh
  ./run_benchmark.sh
```

### Example of work session

<html>
 <table>
  <tr>
  <th>Server</th>
  <th>Client</th>
  </tr>

  <tr>
  <td>
  HYDRA_HOST=$(hostname)<br>
  hydra_nameserver & <br>
  sleep 1
  </td>
  <td>
  &nbsp;
  </td>
  </tr>

  <tr>
  <td>
  cd test<br>
  mpirun -nameserver ${HYDRA_HOST} -np 2 ../bin/mfs_server_mpi -n conf-mpi.yaml &
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
  mpirun -nameserver ${HYDRA_HOST} -np 2 ./test_simple -n conf-mpi.yaml
  </td>
  </tr>
  </table>
</html>

