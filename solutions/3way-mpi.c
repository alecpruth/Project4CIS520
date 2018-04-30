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

#define BUF_SIZE 200*1024*1024
#define LINE_COUNT_MAX 100000
#define LINE_LENGTH_MAX 2001

typedef struct
{
    unsigned    length;
    char        longest_substr[LINE_LENGTH_MAX];
    unsigned    flag;
} result_set;

int main(int argc, char *argv[])
{
    int    i;
    char * longest_substr;
    int fd;
    char *filename;
    char *buf;
    char **line_ptrs;
    char *first_line;
    char *next_line;
    char *next;

    result_set rset;
    
    int rc;
	int numtasks, numWorkers;
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

            MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            //MPI_Get_processor_name(hostname, &length);
        
        //printf("size = %d rank = %d\n", numtasks, rank);

        if ( rank == 0 ) {
            
            //MPI_Bcast(buf, BUF_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD); 
            MPI_Barrier(MPI_COMM_WORLD);
            // Wait for data from slave processes
            while(line < LINE_COUNT_MAX-1) {
                for(i = 1; i < numtasks && line < LINE_COUNT_MAX-1; i++, line++) {
                    MPI_Recv(&rset.length, 1, MPI_UNSIGNED, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                    MPI_Recv(&rset.longest_substr, LINE_LENGTH_MAX, MPI_CHAR, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    printf("[Thread rank %d]<%d> and <%d> : <%.*s>\n", i, line, line+1, rset.length, rset.longest_substr);  
                    //printf("[Thread rank %d]: Length <%d>\n", 1, rset.length);  
                }
            }
        }
        
        else{
            printf("MASTER: Number of MPI task is: %d\n", numtasks);
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
            line_ptrs = malloc( (LINE_COUNT_MAX+1) * sizeof(char *) );  // An extra field for a NULL Pointer
                if(line_ptrs == NULL) {
                    perror("malloc failed!\n");
                    free(buf);
                    exit(-2);
                }
        
            memset(line_ptrs, 0, (LINE_COUNT_MAX+1) * sizeof(char *) );
            
            first_line = strtok_r(buf, "\n", &next);
            line_ptrs[0] = first_line;
            //printf("line 0: <%s>\n", first_line);
            
            for(i = 1, next_line = first_line; next_line != NULL && i < LINE_COUNT_MAX; i++ ) {
                next_line = strtok_r(NULL, "\n", &next);
                line_ptrs[i] = next_line;
                //printf("line %d: <%s>\n", i, next_line);
            }
            
            for(i = rank-1, numWorkers = numtasks-1; line_ptrs[i] != NULL && line_ptrs[i+1] != NULL && i < LINE_COUNT_MAX; i += numWorkers) {
                //printf("line %d: <%s>\n", i, line_ptrs[i]);
                unsigned longest_length = find_longest_substr(line_ptrs[i], line_ptrs[i+1], &longest_substr);
                MPI_Send(&longest_length, 1, MPI_UNSIGNED, 0, rank, MPI_COMM_WORLD); 
                MPI_Send(longest_substr, longest_length, MPI_CHAR, 0, rank, MPI_COMM_WORLD); 
                //printf("[Thread rank %d]<%d> and <%d> : <%.*s>\n", rank, i, i+1, longest_length, longest_substr);   
            }
        }
        
        MPI_Finalize();

    return 0;

}
