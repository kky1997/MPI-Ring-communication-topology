#include <stdio.h> //standard io library
#include <string.h> //string library
#include <stdlib.h> //standard library for the rand() function
#include <time.h> //time library so we can use time() function
#include <mpi.h>

int main(void) 
{
    
    int m; //declare "m" variable
    int comm_sz; //Number of processes
    int my_rank; //My process rank'

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    m = my_rank;

    for(int q = 0; q < 3; q++) //loop 3 times to shift numbers 3 times
    {
        int recieved;
        int destination;
        int source;
        if(my_rank == 0) //check if rank is 0, because if it is then the destination is slightly different
        {
            destination = comm_sz - 1;
            source = my_rank + 1;
            printf("Process %d is sending number %d to process %d\n", my_rank, m, destination);
            MPI_Send(&m, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
            MPI_Recv(&recieved, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            m = recieved;
        }
        else if(my_rank != 0) //otherwise destination is just -1 their local my_rank variable
        {
            destination = my_rank - 1;
            source = (my_rank - 1 + comm_sz) % comm_sz; //use this formula here so that if my_rank == 1 (second highest rank), source will be 0
            printf("Process %d is sending number %d to process %d\n", my_rank, m, destination);
            MPI_Send(&m, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
            MPI_Recv(&recieved, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            m = recieved;
        }
    }

    for(int i = 0; i < comm_sz; i++)
    {
        if(my_rank == i)
        {
            printf("Process %d holds the m-value %d\n", my_rank, m);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
       
    MPI_Finalize();
    return 0;
}