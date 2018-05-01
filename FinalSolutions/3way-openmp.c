#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <omp.h>

#define BUF_SIZE 200*1024*1024
#define MAX_LINES 1000000
#define NUM_THREADS 1

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


unsigned find_longest_substr(char *str1, char *str2, char ** write_to){

unsigned cnt;
unsigned longest_length = 0;
char *ptr1 = str1;
    
    for(; *str2; str1 = ptr1, str2++ ) {
        for(; *str1; str1++ )
        {
            cnt = match_count(str1,str2);
            if(longest_length < cnt ) {
                longest_length = cnt;
                *write_to = str1;
            }
        }
    }
    
    return longest_length;
}

typedef struct{
	char * substr;
	unsigned length;
}resultSet;


void print_results(resultSet * res){
	int i;
	int test = -1;
	for(i = 0; i < MAX_LINES-1; i++,res++){
		printf("<%d> and <%d> : <%.*s>\n",i+1, i+2,res->length,res->substr);
	}
}

void scan_file(char * filename){
	int fd;
	char *buf;
	char **line_ptrs;
	char *first_line;
	char *next_line;
	char *next;
	resultSet * set; 
	
	buf = (char *)malloc(BUF_SIZE);
    if(buf == NULL){
		perror("failed");
		exit(-1);
    }
    line_ptrs = (char **)malloc(MAX_LINES * sizeof(char *));
    if(line_ptrs == NULL){
        perror("malloc failed");
        free(buf);
        exit(-2);
    }
	
	set = (resultSet *)malloc(MAX_LINES * sizeof(resultSet));
    if(set == NULL){
        perror("malloc failed");
        free(buf);
        free(line_ptrs);
        exit(-4);
    }
	
	memset(buf, 0, BUF_SIZE);
    memset(line_ptrs, 0, MAX_LINES * sizeof(char *));
    memset(set, 0, MAX_LINES * sizeof(resultSet));
	
	fd = open(filename, O_RDONLY);
    if(fd == -1) {
        perror("Could not open file!");
    }
    read(fd, buf, BUF_SIZE);
    
    first_line = strtok_r(buf, "\n", &next);
    line_ptrs[0] = first_line;
	
	int i;
    
    for(i = 1, next_line = first_line; next_line != NULL && i < MAX_LINES; i++) {
        next_line = strtok_r(NULL, "\n", &next);
        line_ptrs[i] = next_line;
    }
	
	//omp_set_num_threads(NUM_THREADS);
	
	#pragma omp parallel
	{
		int j;
		resultSet* set_temp;
		char * longest;
		unsigned length;
		int thread_num = omp_get_num_threads();
		#pragma omp private(j,set_temp,longest,length)
		{
			set_temp = set;
			for(j = omp_get_thread_num(), set_temp += j; (j < MAX_LINES) && (j+1 < MAX_LINES); j += thread_num, set_temp += thread_num){
				//printf("<%d> and <%d>\n",j,j+1);
				length = find_longest_substr(line_ptrs[j],line_ptrs[j+1],&longest);
				set_temp->substr = longest;
				set_temp->length = length;
			}
		}
	}
	
	print_results(set);
	
	free(buf);
	free(line_ptrs);
	
	return NULL;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: longsubstr filename\n");
	}
	else
	{
		scan_file(argv[1]);
	}
}