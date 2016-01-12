#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv)
{
  int rc, size, rank;
  MPI_Status status;
 
  rc = MPI_Init(&argc, &argv);
  // 1.MPI_Abort demo: the program will terminate when initializing failed.
  if (rc != MPI_SUCCESS)
  {
    printf("Failed in initializing MPI environment. Aborting...");
	MPI_Abort(MPI_COMM_WORLD, rc);
	return 0;
  }
  
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  // 2.MPI_Comm_rank demo
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("MPI_Comm_rank demo: Processor on rank %d starting...\n", rank); 
  
  // 3.MPI_Send demo
  int value = 0, i;
  if (rank == 0)
  {
    printf("MPI_Send demo - Please input an integer:\n");
    rc = scanf("%d", &value);
    printf("MPI_Send demo - Task %d sends [%d] using MPI_Send\n", rank, value);
	for (i=1; i<size; ++i)
	  MPI_Send(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    printf("MPI_Send demo - Task %d received [%d] using MPI_Recv\n", rank, value);
  }
  
  fflush(stdout);
  // Ensure all tasks have completed previous steps.
  MPI_Barrier(MPI_COMM_WORLD);
  
  // 4.MPI_Isend
  MPI_Request reqs[1];
  MPI_Status stats[1];
  if (rank == 0)
  {
	printf("MPI_Isend demo - Please input an integer:\n");
	rc = scanf("%d", &value);
	for (i=1; i<size; ++i)
	  MPI_Isend(&value, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &reqs[0]);
  }
  else
  {
    MPI_Irecv(&value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &reqs[0]);  
  }
  // Wait for the two non-blocking tasks to complete
  MPI_Waitall(1, reqs, stats);
  if (rank == 0)
  {
    printf("MPI_Isend demo - Task %d sends [%d] using MPI_Isend\n", rank, value);
  }
  else
  {
    printf("MPI_Isend demo - Task %d received [%d] using MPI_Irecv\n", rank, value);
  }
  
  // 5.MPI_Bcast demo
  if (rank == 0)
  {
	printf("MPI_Bcast demo - Please input an integer:\n");
	rc = scanf("%d", &value);
  }
  MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank > 0)
  {
    printf("MPI_Bcast demo - Task %d received [%d] using MPI_Bcast\n", rank, value);
  }
  
  fflush(stdout);
  // Ensure all tasks have completed previous steps.
  MPI_Barrier(MPI_COMM_WORLD);
  
  // 6. MPI_Abort demo (deliberately): enter 0 to abort.
  while(1)
  {
	if (rank == 0)
	{
      printf("MPI_Abort demo - Enter 0 to abort:\n");
	  rc = scanf("%d", &value);
	  if (value == 0)
	  {
	    printf("MPI_Abort demo - Aborting the program deliberately.");
		MPI_Abort(MPI_COMM_WORLD, MPI_ERR_UNKNOWN);
	  }
	}
  }
 
  MPI_Finalize();
  return 0;
}