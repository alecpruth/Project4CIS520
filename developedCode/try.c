#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <omp.h>
//#include <mpi.h>

//#define NUM_THREADS 4
unsigned NUM_THREADS = 1;
#define MAX_LINES 1000000

#define BUF_SIZE 10*1024*1024
#define LINE_COUNT_MAX 3000
#define LINE_LENGTH_MAX 65536

char ** longest_results;

unsigned match_count (char *str1, char *str2) 
{
unsigned i;

    for( i=0; str1[i] != 0 && str2[i] !=0; i++) {
        if(str1[i] != str2[i]) {
            return i;
        }
    }
        
    return i;
}


unsigned find_longest_substr(char *str1, char *str2, char *write_to)
{
unsigned cnt;
unsigned longest_length;
char *ptr1 = str1;
    
	//omp private here too?
    for(; *str2; str1 = ptr1, str2++ ) {
        for(; *str1; str1++ )
        {
            cnt = match_count(str1,str2);
            if(strlen(write_to) < cnt ) {
                longest_length = cnt;
                strncpy(write_to, str1, cnt);
            }
        }
    }
    
    return longest_length;
}

void scan_file(char *filename)
{
int fd;
char *buf;
char **line_ptrs;
char *first_line;
char *next_line;
char *next;
char longest_substr[4096];

    buf = (char *)malloc(BUF_SIZE);
    line_ptrs = (char **)malloc(LINE_COUNT_MAX);
    memset(buf, 0, BUF_SIZE);
    // printf("Filename is <%s>\n", filename);
    fd = open(filename, O_RDONLY);
    if(fd == -1) {
        perror("Could not open file!");
    }
    read(fd, buf, BUF_SIZE);
    
    first_line = strtok_r(buf, "\n", &next);
    line_ptrs[0] = first_line;
    //printf("line 0: <%s>\n", first_line);
    
    for(int i = 1; next_line != NULL; i++ ) {
        next_line = strtok_r(NULL, "\n", &next);
        line_ptrs[i] = next_line;
        //printf("line %d: <%s>\n", i, next_line);
    }
    
    for(int i = 0; line_ptrs[i] != NULL && line_ptrs[i+1] != NULL; i++) {
        //printf("line %d: <%s>\n", i, line_ptrs[i]);
        unsigned longest_length = find_longest_substr(line_ptrs[i], line_ptrs[i+1], &longest_substr[0]);
        printf("<%d> and <%d> : <", i, i+1);
        fflush(stdout);
        write(STDOUT_FILENO, longest_substr, longest_length);
        printf(">\n");
        memset(&longest_substr[0], 0, sizeof(longest_substr));
    }
    
    free(buf);
    free(line_ptrs);
    
}

/*
void scan_file(char ** line, int id)
{
	int i,j,cnt;
	char * longest_substr_temp;
	int start, end;
	

		//Indicates what section of lines the thread is responsible for covering
		//Found in pt1_openmp_critical.c
		//Might need to fix the bug of not working right if sections don't divide cleanly
		start = id * (MAX_LINES/NUM_THREADS);
		end = start + (MAX_LINES/NUM_THREADS);
		
		for(i = start; i < end; i++){
			cnt = find_longest_substr(line[i],line[i+1],longest_substr_temp);
				strncpy(longest_results[i],longest_substr_temp,cnt);
			
		}
		
		if(end != MAX_LINES){
			cnt = find_longest_substr(line[end],line[end+1],longest_substr_temp);
				strncpy(longest_results[end],longest_substr_temp,cnt);
			
		}
}

/*
*	This method reads in the wikipedia lines and initializes the memory for the longest substrings.
*	The code for this method was taken from the file find_keys.c, provided to us by Professor Andresen
*/
/*
char ** init_arrays(char * filename)
{
	int i, nlines, err;
	FILE *fd;
    char ** lines;
	
	longest_results = malloc(MAX_LINES * sizeof(char *));
	for(i = 0; i < MAX_LINES; i++)
	{
		longest_results[i] = malloc(2001);
	}
	
	lines = (char **) malloc(MAX_LINES * sizeof(char *));
	for(i = 0; i < MAX_LINES; i++){
		lines[i] = malloc(2001);
	}
	
	fd = fopen(filename, "r");
	nlines = -1;
	do{
		err = fscanf( fd, "%[^\n]\n", lines[++nlines] );
	}while(err != EOF && nlines < MAX_LINES);
	fclose(fd);
	return lines;
}

void print_results(){
	int i;
	
	for(i = 0; i < MAX_LINES; i++){
		printf("<%d> and <%d> : <%s>\n", i, i+1, longest_results[i]);
	}
}*/

int main(int argc, char *argv[])
{
    int i;
    int rc;
	int numtasks;
    int rank;
	//MPI_Status Status;

  if (argc < 2) {
      printf("Usage: longsubstr filename\n");
  }
  
  else {
      
    //rc = MPI_Init(&argc,&argv);
    
    char ** wiki_lines; 
    
    /*
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    */
    rank = 0;
	NUM_THREADS = numtasks;
    
	printf("size = %d rank = %d\n", numtasks, rank);

	if ( rank == 0 ) {
		//wiki_lines = init_arrays(argv[1]);
       scan_file(argv[1]);
	}
    
    
/*#define NUM_THREADS 4
#define MAX_LINES 1000000

#define BUF_SIZE 10*1024*1024
#define LINE_COUNT_MAX 3000
#define LINE_LENGTH_MAX 65536

char ** longest_results;*/

	//MPI_Bcast(wiki_lines, BUF_SIZE * MAX_LINES, MPI_CHAR, 0, MPI_COMM_WORLD); //Still don't know what params to pass
		
	//scan_file(wiki_lines, rank);

	//MPI_Reduce(longest_results, wiki_lines, LINE_LENGTH_MAX, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); //Still don't know what params to pass


	if ( rank == 0 ) {
		//print_results();
	}

	//MPI_Finalize();
	return 0;
    
	/*omp_set_num_threads(NUM_THREADS);
	
	char ** wiki_lines = init_arrays(argv[1]);
	
	#pragma omp parallel
	{
		scan_file(wiki_lines,omp_get_thread_num());
	}
	
	print_results();
	//free everything?*/
  }


}
