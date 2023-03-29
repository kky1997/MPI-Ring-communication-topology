#include <stdio.h> //standard io library
#include <string.h> //string library
#include <mpi.h>

int main(void) 
{
    
    int m; //declare "m" variable
    int comm_sz; //Number of processes
    int my_rank; //My process rank'

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Barrier(MPI_COMM_WORLD); // wait for all processes to synchronize before proceeding
    
    m = my_rank;

    for(int q = 0; q < 3; q++) //loop 3 times to shift numbers 3 times
    {
      int recieved;
      int destination;
      int source;

      if(my_rank != 0)
      {
        destination = my_rank - 1;
        source = (my_rank - 1 + comm_sz) % comm_sz;
        MPI_Recv(&recieved, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        m = recieved;
      }
      destination = (my_rank + 1) % comm_sz;
      MPI_Send(&m, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);

      if(my_rank == 0)
      {
        source = comm_sz - 1;
        MPI_Recv(&recieved, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }

      MPI_Barrier(MPI_COMM_WORLD); // wait for all processes to synchronize before proceeding
    }

    MPI_Barrier(MPI_COMM_WORLD);
    for(int i = 0; i < comm_sz; i++)
    {
        if(my_rank == i)
        {
            printf("Process %d holds the m-value %d\n", my_rank, m);
        }
        MPI_Barrier(MPI_COMM_WORLD); // wait for all processes to synchronize before proceeding
    }
       
    MPI_Finalize();
    return 0;
}