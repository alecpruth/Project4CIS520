#include <stdio.h>
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

int main()
{
char *str1 = "Hellodidkethis";
char *str2 = "boohooHellossalecdidkethis";
char longest_substr[100];

    //printf("Match count: <%d>!\n", match_count(str1,str2));
    memset(longest_substr, 0, sizeof(longest_substr));
    printf("Match count: <%d>!\n", find_longest_substr(str1, str2, &longest_substr[0]) );
    printf("Longest substring is <%s>\n", longest_substr);
    return 0;
}
