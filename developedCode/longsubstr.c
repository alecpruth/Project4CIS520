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


unsigned find_longest_substr(char *str1, char *str2, char ** write_to)
{
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


void scan_file(char *filename)
{
int fd;
char *buf;
char **line_ptrs;
char *first_line;
char *next_line;
char *next;
char *longest_substr;

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
        int longest_length = find_longest_substr(line_ptrs[i], line_ptrs[i+1], &longest_substr);
        printf("<%d> and <%d> : <", i, i+1);
        fflush(stdout);
        write(STDOUT_FILENO, longest_substr, longest_length);
        printf(">\n");
        
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
