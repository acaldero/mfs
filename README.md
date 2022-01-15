# MPI_PFS (v0.7)
Prototype of MPI_PFS

## Getting started

### Install prerequisites

```
sudo apt-get install -y build-essential
sudo apt-get install -y flex libtool
sudo apt-get install -y mpich mpich-doc
```

### Get MPI_PFS source code

```
git clone https://github.com/acaldero/mpi_pfs.git
cd mpi_pfs
```

### Compile MPI_PFS

```
make clean
make
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
  HYDRA_HOSTNAME=$(hostname)
  hydra_nameserver & <br>
  mpirun -np 2 -nameserver ${HYDRA_HOSTNAME} ./mfs_server &<br>
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
  mpirun -np 2 -nameserver ${HYDRA_HOSTNAME} ./mfs_client
  </td>
  </tr>
  </table>
</html>

