#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define BUF_SIZE 300

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
    

void scan_file(char *filename)
{
int fd;
char buf[BUF_SIZE];
char *word_ptrs[100];
char *first_word;
char *next_word;
char *next;
char longest_substr[100];

    memset(buf, 0, sizeof(buf));
    // printf("Filename is <%s>\n", filename);
    fd = open(filename, O_RDONLY);
    read(fd, buf, sizeof(buf));
    
    
    first_word = strtok_r(buf, " \t\n", &next);
    word_ptrs[0] = first_word;
    //printf("Word 0: <%s>\n", first_word);
    
    for(int i = 1; next_word != NULL; i++ ) {
        next_word = strtok_r(NULL, " \t\n", &next);
        word_ptrs[i] = next_word;
        //printf("Word %d: <%s>\n", i, next_word);
    }
    
    for(int i = 0; word_ptrs[i] != NULL && word_ptrs[i+1] != NULL; i++) {
        //printf("Word %d: <%s>\n", i, word_ptrs[i]);
        memset(longest_substr, 0, sizeof(longest_substr));
        find_longest_substr(word_ptrs[i], word_ptrs[i+1], longest_substr);
        printf("Longest substring between word<%d> and word<%d> is <%s>\n", i, i+1, longest_substr);
    }
    
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
