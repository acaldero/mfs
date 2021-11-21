# MPI_PFS (v0.1)
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
  <th>Server</th>
  <th>Client</th>
  </tr>
  <tr>
  <td>
  mpirun -np 1 ./mfs_server &<br>
  cat mfs_server.port
  tag#0$port#123$description#mirlo$ifname#1.1.1.1$<br>
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

