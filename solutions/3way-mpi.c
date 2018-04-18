#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
    printf("Filename is <%s>\n", filename);
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
