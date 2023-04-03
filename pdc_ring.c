#include <stdio.h> //standard io library
#include <string.h> //string library
#include <mpi.h>
#include <stdlib.h> //import standard library to access srand() and rand() functions
#include <time.h> //import time library so we can use time() function when seeding rand()

//Set a const int as a global variable a this will be unchanged and used by all processes during execution
//This variable sets the max number of characters that can exist in a character array declared below.
const int MAX_STRING = 100;
int main(void) 
{
    char output[MAX_STRING]; //declare a character array with length of 100
    int random_num; //this int variable will hold the random number generated later by the ran() function
    int recieved; //this int variable acts as a buffer and holds the value retrieved at each MPI_Recv() function
    int m = 10; //declare "m" variable, initilize with value of 10 as we want random number from 10 - 20, so rand%11 + 10 will give between 10 - 20
    int comm_sz; //Number of processes, later initilized by MPI_Comm_Size() function
    int my_rank; //My process rank', later initilized by MPI_Comm_rank() function

    //these three MPI functions initilize MPI, retrieve the total number of processes, and rank the current processes
    //MPI_Init() initilizes MPI, we pass in null which indiciates that the program is not passing any command-line arguments
    //MPI_Comm_size() retrieves the number of processes that program is started with, this number of processes is associated with the MPI_COMM_WORLD group and stores the number in the
    //variable comm_sz.
    //MPI_Comm_rank() retrieves the rank of the current process within the group MPI_COMM_WORLD and stores that rank integer in a variable called my_rank
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //need to have a different seed for each process, so we must use an int unique to each process, in this case their rank.
    //then call the rand() function to generate a random number and % by 11 so that we can generate a random number between 0 and 10
    //we then add the random number % 11 to the variable random_num, which was initilized to hold 10.
    //this means that the randomly generated number (0 - 10) will always be at least 10 (0 + 10) or at most 20 (10 + 10).
    srand(time(0) + my_rank); 
    random_num = rand() % 11;
    //m = my_rank;

    //set the m-value for each process to their randomly generated number
    m += random_num;

    //within this for loop is the main logic for this program, this initilizes the ring communication topology and
    //allows the shifting of m-values to the left 3 times (the number of iterations of the loop).
    for(int q = 0; q < 3; q++) //loop 3 times in order to shift numbers 3 times
    {
        int source; //declare a variable to hold the source which will be passed into MPI_Recv() functions
        int destination; //declare a variable to hold the destination passed into MPI_Send() functions

        /*
        the below if and else statements are to determine what the source and destination should be for the particular process. This is done based on the rank of the process:
            if rank == 0:
                If the process has a rank == 0, then it's left neighbour is the process which has the highest rank number (lowest rank processes)
                hence we can get process 0's destination by getting the total number of processes - 1.
                
                The process 0's source will be it's right neighbour, which will always have a rank of 0 + 1 (process 1), so source is determined by
                process 0's rank + 1.

            else:
                all other processes with ranks > 0 have left neighbours (destination) which are just -1 of their rank. 
                Eg. process 1's left neighbour is 0 (1 - 1 = 0).
                Their source (right neighbour) can be determined by their rank + 1 % total number of processes. 
                Eg. process 1's right neighbour and source, when executing this program with 4 processes, would be (1 + 1) % 4 which is 2.
        */
        if(my_rank == 0)
        {
            source = my_rank + 1;
            destination = comm_sz - 1;
        }
        else
        {
            source = (my_rank + 1) % comm_sz;
            destination = my_rank - 1;
        }

        
        //here we split the calling for MPI_Recv() based on whether a process' rank is 0 or not, this is so that all processes do not call Recv() 
        //at the start and all block before an send() calls, which would deadlock the program. First we allow all processes of rank > 0 to make a Recv() call.
        if(my_rank !=0)
        {
            MPI_Recv(&recieved, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d recieved number %d from process %d\n", my_rank, recieved, source);
        }

        //the first process (rank 0), will make the first send() as all other rank's would be blocking after Recv() call
        //this ensures that one send() is made allowing the program to continue.
        printf("Process %d sending number %d to process %d\n", my_rank, m, destination);
        MPI_Send(&m, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);

        //now after rank 0 has done it's send, another process will stop blocking and do it's send. Since process 0 is the only one that has yet to do a Recv() call
        //now we call it, ensuring that when it's right neigbour calls send(), it will be recieved by process 0.
        if(my_rank == 0)
        {
            MPI_Recv(&recieved, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d recieved number %d from process %d\n", my_rank, recieved, source);
        }

        //update each process' m-value with the value they have recieved from their MPI_Recv() call.
        m = recieved;
    }
    
    //The below if and else statements are the main logic for printing the final m-values of each process deterministically in order of their rank.
    //this is done by all processes with rank > 0 creating a string and outputting it to the string buffer "output".
    //they then send this output array to process 0, which is responsible for printing it's own m-value as well as the string sent by all other processes rank > 0.
    if(my_rank != 0)
    {
      sprintf(output,"Process %d holds value %d", my_rank, m);
      MPI_Send(output, strlen(output)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
      printf("Process %d holds value %d\n", my_rank, m);

      //this for loop ensures that the output to console will always be in order of rank as "q" is used to determine the source of the Recv().
      //since "q" starts at 1 and is incremented by 1 only after every iteration, this means that process 0 will always recieve in the order;
      //1, 2, 3 etc.
      for(int q = 1; q < comm_sz; q++) 
      {
        MPI_Recv(output, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%s\n", output);
      }
    }

    //This final MPI function performs a variety of cleanup tasks, including freeing memory and closing any network connections opened by MPI during execution. 
    MPI_Finalize();
    return 0;
}