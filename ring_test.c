#include <stdio.h> //standard io library
#include <string.h> //string library
#include <mpi.h>

const int MAX_STRING = 100;
int main(void) 
{
    char output[MAX_STRING];
    int recieved;
    int m; //declare "m" variable
    int comm_sz; //Number of processes
    int my_rank; //My process rank'

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    m = my_rank;

    for(int q = 0; q < 3; q++) //loop 3 times to shift numbers 3 times
    {
      int destination;
      int source;
      if(my_rank != 0)
      {
        destination = my_rank - 1;
      }
      else if(my_rank == 0)
      {
        destination= comm_sz - 1;
      }

      MPI_Send(&m, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
      if(my_rank == 0)
      {
        MPI_Recv(&recieved, 1, MPI_INT, comm_sz - 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
      else if(my_rank != 0)
      {
        MPI_Recv(&recieved, 1, MPI_INT, my_rank + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
      m = recieved;
    }
    

    if(my_rank != 0)
    {
      sprintf(output,"Process %d holds value %d", my_rank, m);
      MPI_Send(output, strlen(output)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
      printf("Process %d holds value %d", my_rank, m);
      for(int q = 1; q < comm_sz; q++) 
      {
        MPI_Recv(output, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%s\n", output);
      }
    }
       
    MPI_Finalize();
    return 0;
}