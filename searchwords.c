#include <stdio.h>        // for printf(), fopen(), fclose(), getline()
#include <stdlib.h>       // for malloc(), free()
#include <string.h>       // for strdup()

int count_words(char* filename)
{
  FILE *fp;               // file pointer
  char *line = NULL;      // NULL is a pointer to "nothing"
  size_t n = 0;           // will contain the size of the allocated buffer
  ssize_t len;            // will contain the length of line read (or -1)
  int count = 0;          // count number of lines

  fp = fopen(filename , "r");
  if (fp == NULL)       // check if there was an error opening the file
    return 0;

  while ((len = getline(&line, &n , fp)) != -1)
    count++;

  fclose(fp);
  
  return count;
}

char** read_words(char* filename, int numwords)
{
  FILE *fp;               // file pointer
  char *word = NULL;      // NULL is a pointer to "nothing"
  size_t n = 0;           // will contain the size of the allocated buffer
  ssize_t len;            // will contain the length of word read (or -1)
  int i;                  // loop control variable
  // allocate array of words (ie. array of pointers to characters)
  char** words = malloc((numwords+1)*sizeof(char*));  // +1 will append NULL

  fp = fopen(filename , "r");
  if (fp == NULL) {       // check if there was an error opening the file
    fprintf(stderr, "Failed to open file %s\n", filename);
    return NULL;
  }

  for (i = 0; i < numwords; i++) {
    // if *words = NULL and n = 0, then getline() will allocate memory for word
    if ((len = getline(&word, &n , fp)) == -1) {
      fprintf(stderr, "read fewer than %d words\n", numwords);  // EOF
      break;
    }
    word[len-1] = '\0';         // strip off newline
    words[i] = strdup(word);    // need to duplicate line (why?)
  }

  free(word);             // if you don't free word, you get a memory leak
  words[i] = NULL;        // a "sentinel" value for easier processing (how?)
  fclose(fp);             // always good idea to close file when done

  return words;           // return pointer to pointer to 1st char of 1st word
}

void free_words(char** words)
{
  int i = 0;

  while (words[i] != NULL) {
    free(words[i]);
    words[i++] = NULL;    // NOTE the i++ inside the brackets
  }

  // How about the next loop instead of the one above:  (will it work?)
  /*
  while (*words != NULL) {
    free(*words);
    *words++ = NULL;
  }
  */
}

// return index where word is found in words, else -1
// (performing a binary search because words are sorted)
int find_word(char* word, char** words, int numwords)
{
  int first = 0;
  int last = numwords-1;
  int mid;
  int cmpval;
  int count = 0;    // count number of checks made

  while (first <= last) {
    mid = (first + last) / 2;
    printf("\t%2d checking word %s at pos %d\n", ++count, words[mid], mid);
    cmpval = strcmp(word, words[mid]);
    if (cmpval < 0)
      last = mid-1;
    else if (cmpval > 0)
      first = mid+1;
    else
      break;
  }

  return (first <= last) ? mid : -1;
}

int main(int argc, char* argv[])
{
  char** words;            // array of words read
  int numwords;            // number of words read
  int pos;                 // position where word found
  //char* word = "radical";  // word to search for
  char* word = "Brillion";  // word to search for

  if (argc < 2) {
    fprintf(stderr, "USAGE: %s words_filename\n", argv[0]);
    return 1;
  }

  numwords = count_words(argv[1]);
  printf("word count = %d\n", numwords);

  words = read_words(argv[1], numwords);

  pos = find_word(word, words, numwords);
  if (pos != -1) {
    printf("word '%s' found at position %d\n", word, pos);
  }
  else {
    printf("word '%s' not found\n", word);
  }

  free_words(words); // get rid of space for each word
  free(words);       // why do I need to do this in addition to free_words()?

  return 0;          // returning 0 from main indicates no error
}
