#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h> //will need for data analysis?
#include <mpi.h>


#define SIZEOF_WIKI_ENTRIES 1000000000 /*The number of wiki entries*/
#define WIKI_STRINGS 5000              /*The length of a chars in the entries. big arbitrary num
                                        * because of newlines and ending chars. */
#define WORDS_FOUND 100000             /*The number of words to found. Number was a guess */
#define WORDS_LENGTH 26                /*The length of the words found. */


//The array to hold wiki info
char wiki_array[SIZEOF_WIKI_ENTRIES][WIKI_STRINGS];

//words to find
char words_array[WORDS_FOUND][WORDS_LENGTH];


void readWiki(){

  //keep track of what line in
  int lineNum = 0;
  char * line = malloc(WIKI_STRINGS);
  // read in the wiki txt file
  char *fileName = "C:\Users\alecp\Documents\CIS520\625\wiki_dump.txt" //Path will need to be different
  FILE* file = fopen(fileName, "r");

  if(file == NULL){
    printf("Error in opening file");
    return;
  }
  while(fgets(line, WIKI_STRINGS, file) != NULL){
    strcpy(wiki_array[lineNum],  line);
    lineNum++;
  }
  fclose(file);
  free(line);

}

void findSubString(){

  int i, j;
    int found_word = 0; // Assume that the word has not been found (false).

    for(i = 0; i < WORDS_ARRAY_SIZE; i++)
    {
      found_word = 0; // Assume that the word has not been found (false).
      for(j = 0; j < WIKI_ARRAY_SIZE; j++)
      {
        /*strstr is a C standard library string function as defined in string.h.
        strstr() has the function signature
        char * strstr(const char *haystack, const char *needle);
        which returns a pointer to a character at
        the first index where needle is in haystack, or NULL if not present.
        wiki_array = haystack; words_array = needle; */

        char *wordsInWiki = strstr(wiki_array[j], words_array[i]);
        if(wordsInWiki)
        {
          // If this is the first time that the word has been found
          if (found_word == 0)
          {
            // Set found_word to true. Print out the word alongside its line number.
            found_word = 1;
            printf("%s: %d", words_array[i], j + 1);
          }
          // Else, the word has been found before. Append it to the existing string.
          else
          {
            printf(", %d", j + 1);
          }
        }
      }
      if (found_word) printf("\n"); // Get ready for the next word by adding a newline.
  }

}



//found this in pt2.c
int main(int argc, char *argv[]){

  int i, rc;
  int numtasks, rank;
  MPI_Status Status;


  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  NUM_THREADS = numtasks;
  printf("size = %d rank = %d\n", numtasks, rank);
  fflush(stdout);

  if ( rank == 0 ) {
    init_arrays();
  }
  MPI_Bcast(char_array, ARRAY_SIZE * STRING_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

  count_array(&rank);

  MPI_Reduce(local_char_count, char_counts, ALPHABET_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


  if ( rank == 0 ) {
    print_results();
  }

  MPI_Finalize();
  return 0;


}
