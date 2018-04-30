#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define BUF_SIZE 10*1024*1024
#define LINE_COUNT_MAX 10000
#define NUM_THREADS 2

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
    char * longest_substr;
    unsigned length;
    
    
}resultSet;


typedef struct{
    
    int rank;
    char ** line_ptrs;
    resultSet * set;
    
} workerThread;

void * workFunction(void * args){
    
    workerThread * wt;
    resultSet * set = NULL;
    int i;
    char *longest_substr;
    
    wt = (workerThread *)args;
    set = wt->set;
    
    for(i = wt->rank-1, set += i; wt->line_ptrs[i] != NULL && wt->line_ptrs[i+1] != NULL; i += NUM_THREADS, set += NUM_THREADS){
        //printf("line %d: <%s>\n", i , wt->line_ptrs[i]);
        unsigned longest_length = find_longest_substr(wt->line_ptrs[i], wt->line_ptrs[i+1], &longest_substr);
        set->longest_substr = longest_substr;
        set->length = longest_length;
        //getchar();
    } 
    
    return NULL;
}
  
void scan_file(char *filename)
{
int fd;
char *buf;
char **line_ptrs;
char *first_line;
char *next_line;
char *next;
pthread_t workThreads[NUM_THREADS];
workerThread * wt;
resultSet * set; 


    buf = (char *)malloc(BUF_SIZE);
    if(buf == NULL){
        perror("failed");
        exit(-1);
    }
    line_ptrs = (char **)malloc(LINE_COUNT_MAX * sizeof(char *));
    if(line_ptrs == NULL){
        perror("malloc failed");
        free(buf);
        exit(-2);
    }
    
    wt = (workerThread *)malloc(NUM_THREADS * sizeof(workerThread));
    if(wt == NULL){
        perror("malloc failed");
        free(buf);
        free(line_ptrs);
        free(wt);
        exit(-3);
    }
    
    set = (resultSet *)malloc(LINE_COUNT_MAX * sizeof(resultSet));
    if(set == NULL){
        perror("malloc failed");
        free(buf);
        free(line_ptrs);
        exit(-4);
    }
    
    memset(buf, 0, BUF_SIZE);
    memset(line_ptrs, 0, LINE_COUNT_MAX * sizeof(char *));
    memset(set, 0, LINE_COUNT_MAX * sizeof(resultSet));
    // printf("Filename is <%s>\n", filename);
    fd = open(filename, O_RDONLY);
    if(fd == -1) {
        perror("Could not open file!");
    }
    read(fd, buf, BUF_SIZE);
    
    first_line = strtok_r(buf, "\n", &next);
    line_ptrs[0] = first_line;
    //printf("line 0: <%s>\n", first_line);
    
    int i;
    
    for(i = 1, next_line = first_line; next_line != NULL && i < LINE_COUNT_MAX; i++) {
        next_line = strtok_r(NULL, "\n", &next);
        line_ptrs[i] = next_line;
        //printf("line %d: <%s>\n", i, next_line);
    }
    
    for(i = 0; i < NUM_THREADS; i++){
        wt[i].rank = i+1;
        wt[i].line_ptrs = line_ptrs;
        wt[i].set = set;
        
        if(pthread_create(&workThreads[i], NULL, workFunction, (void *)&wt[i])){
            perror("pthread failed");
        }
    }
    

    for( i = 0; i < LINE_COUNT_MAX; i++, set++){
        while(!set->length);
        printf("<%d> and <%d> : <%.*s>\n", i, i+1, set->length, set->longest_substr);
    }
    
    free(buf);
    free(line_ptrs);
    
}

int main(int argc, char *argv[])
{

  if (argc < 2) {
      printf("Usage: longsubstr filename\n");
  }
  
  else {
    scan_file(argv[1]);
  }


}
