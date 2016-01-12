#include <stdio.h>
#include <time.h>
#include "mpi.h"

#define N 1100 // Number of locations (0 -1099)
#define PERIOD 60 // Execution time (in seconds)

int main(int argc, char** argv)
{
  int size, rank, i;  
  MPI_Status status;

  // Initialize and acquire size and current rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if(size < 2)
  {
    printf("Processor number must be greater than 1. Aborting...");
	MPI_Abort(MPI_COMM_WORLD, MPI_ERR_UNKNOWN);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // round: used to record current loop cycles;
  // launches: record the total launches in the period
  // loc: location of each vessel (0 - 1099)
  int round = 0, launches = 0, loc;
  // The array is used to record the location occurrences and launches for each round
  // The first N integers record the occurrence of each location (0 - 1099);
  // The last one spot is used to record interim launches (see details in the Report) 
  int occurrence[N + 1] = {0};
  
  // Initialize random seed, different for each rank
  srand(time(NULL) + rank);
  
  if(rank == 0)
  {
    printf("Start computing on [%d] vessels...\n", size);
  }
  
  // Record the start time
  time_t t_start, t_finish;
  time(&t_start);

  // Start loops
  while(1)
  {
    // Clean last round records on last task
    if (round > 0 && rank == size - 1)
	{
	  occurrence[loc] = 0;
	}
	
    // Allocate locations randomly (0 - 1099)
    loc = rand() % N;

	// Each processor, except the last one, receive interim occurrences and launches from next rank (rank + 1);
    if (rank < size - 1)
    { 
      // Array 'occurrence' is passed from next rank (rank + 1). 
      // Use 'round' as tag to ensure it receive the data in current round	  
      MPI_Recv(occurrence, N + 1, MPI_INT, rank + 1, round, MPI_COMM_WORLD, &status);
	  
	  // The last integer records the interim launches. 
	  // If the occurrence at 'loc' equals to 2, it means there will be one more successful launch 
	  // after counting current processor (See details in Report).
      if (occurrence[loc] == 2)
      {
        ++occurrence[N];
      }
	  // Record this occurrence
      ++occurrence[loc];

	  // Update the total launches on the first processor and display the interim info
      if(rank == 0)
      {
        launches += occurrence[N];
        printf("Round %d - Total Launches: %d", round, launches);
        printf("\n\033[F\033[J");
      }
    }

	// Initialize the 'occurrence' array on the last processor
	if (rank == size - 1)
    {
      // Record the location on last processor      
      ++occurrence[loc];
    }
	
	// Each processor, except the first one, sends the interim occurrence data to previous processor (rank - 1).
    if (rank > 0)
    {
      MPI_Send(occurrence, N + 1, MPI_INT, rank - 1, round, MPI_COMM_WORLD);
    }
  
    // Check execution time. End the loop if exceeding the given period
    time(&t_finish);
    if(t_finish - t_start >= PERIOD)
      break;

	// Increase the round number  
    ++round;
  }
  
  // Display the final launches on the first processor.
  if (rank ==0)
  {
    printf("In a minute there are %d launches in %d rounds.\n", launches, round);
  }

  MPI_Finalize();
  return 0;
}
