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

#define BUF_SIZE 200*1024*1024
#define LINE_COUNT_MAX 100000
#define THREADS_COUNT 16


typedef struct
{
    unsigned    length;
    char *      longest_substr;
    unsigned    flag;
} result_set;


typedef struct __attribute__((packed))
{
    int           rank;
    char **       line_ptrs;
    result_set  * rset;
} wthread_args;


void * worker_fn(void * args)
{
wthread_args * wargs;
result_set   * rset = NULL;
int    i;
char * longest_substr;

    wargs = (wthread_args *)args;
    rset =  wargs->rset;
    
    for(i = wargs->rank-1, rset += i; 
            wargs->line_ptrs[i] != NULL && wargs->line_ptrs[i+1] != NULL && i < LINE_COUNT_MAX; 
                i += THREADS_COUNT, rset += THREADS_COUNT) {
        //printf("line %d: <%s>\n", i, wargs->line_ptrs[i]);
        unsigned longest_length = find_longest_substr(wargs->line_ptrs[i], wargs->line_ptrs[i+1], &longest_substr);
        rset->length = longest_length;
        rset->longest_substr = longest_substr;
        rset->flag = 1;
        //printf("[Thread rank %d]<%d> and <%d> : <%.*s>\n", wargs->rank, i, i+1, longest_length, longest_substr);     
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
int i;
pthread_t worker_thread[THREADS_COUNT];
wthread_args * wargs;
result_set * rset, * nextset;

    buf = (char *)malloc(BUF_SIZE);
    if(buf == NULL) {
        perror("malloc failed!\n");
        exit(-1);
    }
        
    line_ptrs = malloc( (LINE_COUNT_MAX+1) * sizeof(char *) );  // An extra field for a NULL Pointer
    if(line_ptrs == NULL) {
        perror("malloc failed!\n");
        free(buf);
        exit(-2);
    }
    
    wargs = (wthread_args *)malloc( THREADS_COUNT * sizeof(wthread_args) );
    if(wargs == NULL) {
        perror("malloc failed!\n");
        free(buf);
        free(line_ptrs);
        exit(-3);
    }
    
    rset = (result_set *)malloc( LINE_COUNT_MAX * sizeof(result_set) );
    if(rset == NULL) {
        perror("malloc failed!\n");
        free(buf);
        free(line_ptrs);
        free(wargs);
        exit(-4);
    }
    
    memset(buf, 0, BUF_SIZE);
    memset(line_ptrs, 0, (LINE_COUNT_MAX+1) * sizeof(char *) );
    memset(rset, 0,  LINE_COUNT_MAX * sizeof(result_set) );
    // printf("Filename is <%s>\n", filename);
    fd = open(filename, O_RDONLY);
    if(fd == -1) {
        perror("Could not open file!");
    }
    read(fd, buf, BUF_SIZE);
    
    first_line = strtok_r(buf, "\n", &next);
    line_ptrs[0] = first_line;
    //printf("line 0: <%s>\n", first_line);
    
    for(i = 1, next_line = first_line; next_line != NULL && i < LINE_COUNT_MAX; i++ ) {
        next_line = strtok_r(NULL, "\n", &next);
        line_ptrs[i] = next_line;
        //printf("line %d: <%s>\n", i, next_line);
    }
    
    for(i = 0; i < THREADS_COUNT; i++ ) {
        wargs[i].rank = i+1;
        wargs[i].line_ptrs = line_ptrs;
        wargs[i].rset = rset;
        
        if(pthread_create(&worker_thread[i], NULL, worker_fn, (void *)&wargs[i])) {
            perror("pthread_create failed!\n");
        }
    }
    
    /*
    for(i = 0; i < THREADS_COUNT; i++ ) {
        pthread_join(worker_thread[0], NULL);  // Wait for child threads to finish
    }
    */
    
    for(i = 0, nextset = rset; i < LINE_COUNT_MAX-1; i++, nextset++ ) {
        while(!nextset->flag);
        printf("<%d> and <%d> : <%.*s>\n", i, i+1, nextset->length, nextset->longest_substr);
    }
        
    free(buf);
    free(line_ptrs);
    free(wargs);
    free(rset);
}

int main(int argc, char *argv[])
{

  if (argc < 2) {
      printf("Usage: longsubstr filename\n");
  }
  
  else {
    scan_file(argv[1]);
  }

  return 0;
}
