#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <omp.h>

#define NUM_THREADS 4

#define BUF_SIZE 10*1024*1024
#define LINE_COUNT_MAX 3000
#define LINE_LENGTH_MAX 65536

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


void scan_file(char *filename, int threadID)
{
int fd;
char *buf;
char **line_ptrs;
char *first_line;
char *next_line;
char *next;
char *longest_substr;
int i;

	#pragma omp private (fd, buf, line_ptrs,first_line,next_line,next,longest_substr,i)
	{
		buf = (char *)malloc(BUF_SIZE);
		line_ptrs = (char **)malloc(LINE_COUNT_MAX);
		longest_substr = (char *)malloc(LINE_LENGTH_MAX);
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
		
		for(i = 1; next_line != NULL; i++ ) {
			next_line = strtok_r(NULL, "\n", &next);
			line_ptrs[i] = next_line;
			//printf("line %d: <%s>\n", i, next_line);
		}
		
		for(i = 0; line_ptrs[i] != NULL && line_ptrs[i+1] != NULL; i++) {
			//printf("line %d: <%s>\n", i, line_ptrs[i]);
			memset(longest_substr, 0, LINE_LENGTH_MAX);
			find_longest_substr(line_ptrs[i], line_ptrs[i+1], longest_substr);
			printf("<%d> and <%d> : <%s>\n", i, i+1, longest_substr);
		}
		
		free(buf);
		free(line_ptrs);
		free(longest_substr);
	}
    
}

int main(int argc, char *argv[])
{

  if (argc < 2) {
      printf("Usage: longsubstr filename\n");
  }
  
  else {
	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel
	{
		scan_file(argv[1],omp_get_thread_num());
	}
    //scan_file(argv[1]);
  }


}
