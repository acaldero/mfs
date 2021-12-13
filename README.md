# MPI_PFS (v0.7)
Prototype of MPI_PFS

## Prerequisites

 * MPICH 3.3.x

## Getting MPI_PFS

```
git clone https://github.com/acaldero/mpi_pfs.git
cd mpi_pfs
make
```

## Example of work session

<html>
 <table>
  <tr>
  <th>Server (IP1)</th>
  <th>Client (IP2)</th>
  </tr>
  <tr>
  <td>
  hydra_nameserver & <br>
  mpirun -np 2 -nameserver IP1 ./mfs_server &<br>
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
  mpirun -np 2 -nameserver IP1 ./mfs_client
  </td>
  </tr>
  </table>
</html>

