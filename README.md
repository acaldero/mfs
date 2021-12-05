# MPI_PFS (v0.7)
Proof of concept + prototype of MPI_PFS

## Prerequisites

 * MPICH 

## Getting MPI_PFS

```
git clone https://github.com/acaldero/mpi_pfs.git
cd mpi_pfs
Make
```

## Example of work session

<html>
 <table>
  <tr>
  <th>Server(s)</th>
  <th>Client(s)</th>
  </tr>
  <tr>
  <td>
  mpirun -np 2 ./mfs_server &<br>
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
  mpirun -np 2 ./mfs_client
  </td>
  </tr>
  </table>
</html>

