#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <omp.h>
#include <mpi.h>

#define NUM_THREADS 4
#define MAX_LINES 1000000

#define BUF_SIZE 10*1024*1024
#define LINE_COUNT_MAX 3000
#define LINE_LENGTH_MAX 65536



int main(int argc, char *argv[])
{
    int i;
    int rc;
    int length;
	int numtasks;
    int rank;
	char hostname[MPI_MAX];
    
    

      
    rc = MPI_Init(&argc,&argv);
    
    
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Get_processor_name(hostname, &length);

	NUM_THREADS = numtasks;
    
	//printf("size = %d rank = %d\n", numtasks, rank);

	if ( rank == MASTER ) {
		printf("MASTER: Number of MPI task is: %d\n", numtasks);
	}
    
    else{
        workerFunction();
    }
    
    MPI_Finalize();


}
