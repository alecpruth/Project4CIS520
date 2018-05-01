#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include "helperfn.c"
#include <mpi.h>

#define BUF_SIZE 1000*1024*1024
#define LINE_COUNT_MAX 10000
#define LINE_LENGTH_MAX 2001

typedef struct
{
    unsigned    length;
    char        longestSub[LINE_LENGTH_MAX];
    unsigned    flag;
} result_set;

int main(int argc, char *argv[])
{
    int    i;
    char * longestSub;
    int fd;
    char *filename;
    char *buf;
    char **ptrsOfLines;
    char *firstLine;
    char *nextLine;
    char *next;

    result_set rset;
    
    int rc;
	int numberOfTasks, numberOfWorkers;
    int rank;
    unsigned line = 0;
    
	//char hostname[MPI_MAX];
    
        if (argc < 2) {
          printf("Usage: longsubstr filename\n");
          return -1;
        }

        filename = argv[1];
        rc = MPI_Init(&argc,&argv);
        
        if (rc != MPI_SUCCESS) {
          printf ("Error starting MPI program. Terminating.\n");
              MPI_Abort(MPI_COMM_WORLD, rc);
            }

            MPI_Comm_size(MPI_COMM_WORLD,&numberOfTasks);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            //MPI_Get_processor_name(hostname, &length);
        
        //printf("size = %d rank = %d\n", numberOfTasks, rank);

        if ( rank == 0 ) {
            
            //MPI_Bcast(buf, BUF_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD); 
            MPI_Barrier(MPI_COMM_WORLD);
            // Wait for data from slave processes
            while(line < LINE_COUNT_MAX-1) {
                for(i = 1; i < numberOfTasks && line < LINE_COUNT_MAX-1; i++, line++) {
                    MPI_Recv(&rset.length, 1, MPI_UNSIGNED, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                    MPI_Recv(&rset.longestSub, LINE_LENGTH_MAX, MPI_CHAR, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    printf("[Thread rank %d]<%d> and <%d> : <%.*s>\n", i, line, line+1, rset.length, rset.longestSub);  
                    //printf("[Thread rank %d]: Length <%d>\n", 1, rset.length);  
                }
            }
        }
        
        else{
            printf("MASTER: Number of MPI task is: %d\n", numberOfTasks);
            buf = (char *)malloc(BUF_SIZE);
                if(buf == NULL) {
                    perror("malloc failed!\n");
                    exit(-1);
                }
            memset(buf, 0, BUF_SIZE);
            fd = open(filename, O_RDONLY);
                if(fd == -1) {
                    perror("Could not open file!");
                }
            
            read(fd, buf, BUF_SIZE);
            
            MPI_Barrier(MPI_COMM_WORLD);
            //MPI_Bcast(buf, BUF_SIZE , MPI_CHAR, 0, MPI_COMM_WORLD); 
            //printf("Rank: <%d> <%.*s>\n", rank, BUF_SIZE, buf);
            //printf("Slave process\n");
            //MPI_Finalize();
            //return 0;
            ptrsOfLines = malloc( (LINE_COUNT_MAX+1) * sizeof(char *) );  // An extra field for a NULL Pointer
                if(ptrsOfLines == NULL) {
                    perror("malloc failed!\n");
                    free(buf);
                    exit(-2);
                }
        
            memset(ptrsOfLines, 0, (LINE_COUNT_MAX+1) * sizeof(char *) );
            
            firstLine = strtok_r(buf, "\n", &next);
            ptrsOfLines[0] = firstLine;
            //printf("line 0: <%s>\n", firstLine);
            
            //paper town nextLine = firstLine;
            
            for(i = 1; nextLine != NULL && i < LINE_COUNT_MAX; i++ ) {
                nextLine = strtok_r(NULL, "\n", &next);
                ptrsOfLines[i] = nextLine;
                //printf("line %d: <%s>\n", i, nextLine);
            }
            
            for(i = rank-1, numberOfWorkers = numberOfTasks-1; ptrsOfLines[i] != NULL && ptrsOfLines[i+1] != NULL && i < LINE_COUNT_MAX; i += numberOfWorkers) {
                //printf("line %d: <%s>\n", i, ptrsOfLines[i]);
                unsigned longest_length = find_longest_substr(ptrsOfLines[i], ptrsOfLines[i+1], &longestSub);
                MPI_Send(&longest_length, 1, MPI_UNSIGNED, 0, rank, MPI_COMM_WORLD); 
                MPI_Send(longestSub, longest_length, MPI_CHAR, 0, rank, MPI_COMM_WORLD); 
                //printf("[Thread rank %d]<%d> and <%d> : <%.*s>\n", rank, i, i+1, longest_length, longestSub);   
            }
        }
        
        MPI_Finalize();

    return 0;

}
