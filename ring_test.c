#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) 
{
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Initialize variable m to the process rank
  int m = world_rank;

  // Perform left circular shift of m-values three times around the ring
  for (int i = 0; i < 3; i++) {
    int dest = (world_rank + 1) % world_size;
    int source = (world_rank - 1 + world_size) % world_size;
    int recv_m;
    MPI_Send(&m, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    MPI_Recv(&recv_m, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    m = recv_m;
  }

/*
  // Output m-values in order of rank
  for (int i = 0; i < world_size; i++) {
    if (world_rank == i) {
      printf("Process %d has m-value %d\n", world_rank, m);
    }
    MPI_Barrier(MPI_COMM_WORLD); // synchronize processes
  }
*/
  MPI_Finalize();
  return 0;
}