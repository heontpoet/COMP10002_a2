// This code is written by Ian Teh Wen Jing on the 22nd of May 2019 for
// COMP10002 assignment 2

// Algorithms are fun!

  #include <stdio.h>
  #include <stdlib.h>
  #include <ctype.h>
  #include <string.h>
  #include <assert.h>

// Stage numbers code provided by the University of Melbourne
#define STAGE_NUM_ONE 1
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4

#define STAGE_H "\n=========================Stage %d=========================\n"

#define MAX_NUM_WORDS 100
#define WORDSTRLEN 20
#define FORMSTRLEN 92
#define HASH_MARKER '#'

// These 2 definitions are provided to me by Professer Allistar Moffet of the
// University of Melbourne
#define BS_NOT_FOUND (-1)
#define BS_FOUND 0

typedef char wordstr_t[WORDSTRLEN+1]; // Stores a word
typedef char formstr_t[FORMSTRLEN+1]; // Stores the forms
typedef char data_t[FORMSTRLEN+1];

typedef struct {                      // Structure to store a dictionary entry
  int sentiment_score;
  wordstr_t base_word;
  formstr_t forms;
}dict_entry_t;

typedef struct {                     // Structure to store dictionary
  int num_words;
  dict_entry_t dict_entries[MAX_NUM_WORDS];
}dictionary_t;

// The next 12 lines of code are provided to me by Professer Allistar Moffet of
// the University of Melbourne
typedef struct node node_t;

struct node {
	data_t data;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

// Function declarations
void print_stage_header(int stage_num);
void stage_one(dictionary_t *dictionary);
void stage_two(dictionary_t *dictionary);
void stage_three(list_t *list);
void stage_four(list_t *list, dictionary_t *dictionary);

void readword(dict_entry_t *dict_entry, int max_len, int stage);
void readforms(dict_entry_t *dict_entry);
void readscore(dict_entry_t *dict_entry);
double avg_characters(dictionary_t *my_dictionary);
double avg_score(dictionary_t *my_dictionary);
int getword(wordstr_t word, int max_len);
int cmpfunc(const void *word1, const void *word2);
void read_into_array(dictionary_t *my_dictionary, char** word_array);

// The functions below are provided to me by Professer Allistar Moffet of the
// University of Melbourne
list_t *make_empty_list(void);
void free_list(list_t *list);
list_t *insert_at_foot(list_t *list, data_t value);
int binary_search(char** word_array, int lo, int hi, char **key);

// Main Function
int
main(int argc, char *argv[]) {
  list_t *sentence_word_list; // Creates a linked list to store the sentence
  dictionary_t my_dictionary; // Creates the sentiment dictionary
  sentence_word_list = make_empty_list();
  my_dictionary.num_words=0;
  stage_one(&my_dictionary);
  stage_two(&my_dictionary);
  stage_three(sentence_word_list);
  stage_four(sentence_word_list, &my_dictionary);
  free_list(sentence_word_list);
  return 0;
}

// Stage 1
void
stage_one(dictionary_t *dictionary) {
  print_stage_header(STAGE_NUM_ONE);
  int stage =0;

  // Creates a new dictionary entry to store the base word, score and forms
  dict_entry_t new_dict_entry;

  // The 3 functions below fill up the dictionary entry structure with the
  // correct information
  readword(&new_dict_entry, WORDSTRLEN, stage);
  readscore(&new_dict_entry);
  readforms(&new_dict_entry);

  printf("First word: %s\n", (&new_dict_entry)->base_word);
  printf("Sentiment score: %d\n",(&new_dict_entry)->sentiment_score);
  printf("Forms: %s\n\n", (&new_dict_entry)->forms);

  // Stores the dictionary entry into the dictionary structure
  dictionary -> dict_entries[dictionary -> num_words] = new_dict_entry;
  dictionary -> num_words++;
}

// Stage 2
void
stage_two(dictionary_t *dictionary) {
  print_stage_header(STAGE_NUM_TWO);
  int ch=0,stage = 1;

  // This loop reads through the rest of the input file provided and stores the
  // dictionary entries into the dictionary structure
  while ((ch=getchar()) == HASH_MARKER) {
    dict_entry_t new_dict_entry;
    readword(&new_dict_entry, WORDSTRLEN, stage);
    readscore(&new_dict_entry);
    readforms(&new_dict_entry);
    dictionary -> dict_entries[dictionary -> num_words] = new_dict_entry;
    dictionary -> num_words++;
  }

  printf("Number of words: %d\n", dictionary->num_words);
  printf("Average number of characters: %.2f\n",avg_characters(dictionary));
  printf("Average sentiment score: %.2f\n\n", avg_score(dictionary));
}

// Stage 3
void
stage_three(list_t *list) {
  print_stage_header(STAGE_NUM_THREE);
  data_t word;
  wordstr_t str;

  // Reads each individual word in the sentence given in the file and stores
  // it into a linked list
  while (getword(str, FORMSTRLEN)!= EOF) {
    strcpy(word, str);
    list = insert_at_foot(list, word);
  }

  node_t *temp = list->head;
  while (temp) {
    printf("%s\n", temp->data);
    temp = temp->next;
  }
  printf("\n");
}

// Stage 4
void
stage_four(list_t *list, dictionary_t *my_dictionary) {
  print_stage_header(STAGE_NUM_FOUR);

  char *word, **word_ptr;
  int j,score=0;
  char** word_array;

  // Creates an array containing each base word from the dictionaru
  word_array =(char **)malloc(sizeof(char)*WORDSTRLEN*my_dictionary->num_words);
  read_into_array(my_dictionary, word_array);

  node_t *temp = list->head;

  // This loops runs through each word in the linked list and searches for it
  // in the array through a binary search before printing it out
  while (temp) {
    word = temp->data;
    word_ptr = &word;
    if (binary_search(word_array, 0, my_dictionary->num_words,
      word_ptr)== BS_NOT_FOUND) {
      printf("%-26s %d\n", temp->data, 0);
    } else {
      for (j=0; j< my_dictionary->num_words; j++) {
        if (strcmp(temp->data, my_dictionary->dict_entries[j].base_word)==0) {
          printf("%-26s %d\n", temp->data,
          my_dictionary->dict_entries[j].sentiment_score);
          score += my_dictionary->dict_entries[j].sentiment_score;
        }
      }
    }
    temp = temp->next;
  }
  printf("Overall score: %13d\n\n", score);
  free(word_array);
}

// Functions

// Reads the base word and stores it into the dictionary entry
void
readword(dict_entry_t *dict_entry, int max_len, int stage) {
  int k,i=stage;
  char *str = malloc(sizeof(char) * WORDSTRLEN);
  k=getchar();
  if (stage == 1) {
    str[0] = k;
  }
  while (((k= getchar())!= EOF)&& (k!= '\n')&& (k!= '\r')&& (k!= '\0')) {
    if (i<max_len) {
      str[i++] = k;
    }
  str[i]='\0';
  }
  strcpy(dict_entry->base_word , str);
  free(str);
}

// Reads the sentiment score of the base word and stores it into a dictionary
// entry
void
readscore(dict_entry_t *dict_entry) {
  int c, i=0, score;
  wordstr_t scorestr;
  while (((c= getchar())!= EOF)&& (c!= '\n')&& (c!= '\r')&& (c!= '\0')) {
      scorestr[i++] = c;
  }
  score = atoi(scorestr);
  dict_entry -> sentiment_score = score;
}

// Reads the form of each base word and stores it into the dictionary entry
void
readforms(dict_entry_t *dict_entry) {
  int k,i=0;
  k=getchar();
  while (((k= getchar())!= EOF)&& (k!= '\n')&& (k!= '\r')&& (k!= '\0')) {
    if (i<FORMSTRLEN) {
      dict_entry -> forms[i++] = k;
    }
  }
  dict_entry -> forms[i]='\0';
}

// Calculates the average number of characters of the base words
// in the dictionary
double
avg_characters(dictionary_t *my_dictionary) {
  int i;
  double total_ch=0.00, avg=0.00;
  // This loop iterates through the dictionary and adds up the length of each
  // base word
  for (i=0; i<=(my_dictionary->num_words); i++) {
    total_ch += strlen(my_dictionary->dict_entries[i].base_word);
  }
  avg = (total_ch)/((double)(my_dictionary->num_words));
  return avg;
}

// Calculates the average sentiment score of the words in the dictionary
double
avg_score(dictionary_t *my_dictionary) {
  int i;
  double total_sc=0.00, avg=0.00;
  // This loop cycles through each entry in the dictionary structure and
  // adds up the total sentiment score
  for (i=0; i<=(my_dictionary->num_words); i++) {
    total_sc += (my_dictionary->dict_entries[i].sentiment_score);
  }
  avg = (total_sc)/((double)(my_dictionary->num_words));
  return avg;
}

// Function to get a single word from a sentence and store it as an array of
// characters
int
getword(wordstr_t word, int max_len) {
  int k, len=0;
  // This loop skips over any element that isn't a alphabet
  while ((k=getchar())!=EOF && !isalpha(k)) {
    // Do nothing more
  }

  if (k==EOF) {
    return EOF;
  }
  // First character of the word has been found
  word[len++] = k;
  while (len < max_len && ((k=getchar())!=EOF && isalpha(k))) {
    word[len++] = k;
  }
  word[len] = '\0';
  return 0;
}

// Function to compare 2 words to find if they are the same
int
cmpfunc(const void *word1, const void *word2) {
  return(strcmp(*(char **)word1, *(char**)word2));
}

// Function to read the base words from the dictionary strucutre into an array
void
read_into_array(dictionary_t *my_dictionary, char** word_array) {
  int i;
  for (i=0; i< my_dictionary->num_words ; i++) {
    word_array[i]=my_dictionary->dict_entries[i].base_word;
  }
  word_array[i]=NULL;
}

// This function is provided to my by the University of
// Melbourne during assignment 1
void
print_stage_header(int stage_num) {
	printf(STAGE_H, stage_num);
}

// The functions below are provided by Professer Allistar Moffet of the
// University of Melbourne
list_t
*make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

void
free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

list_t
*insert_at_foot(list_t *list, data_t value) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	strcpy(new->data, value);
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

int
binary_search(char** A, int lo, int hi,
		char **key) {
	int mid, outcome;
	/* if key is in A, it is between A[lo] and A[hi-1] */
	if (lo>=hi) {
		return BS_NOT_FOUND;
	}
	mid = (lo+hi)/2;
	if ((outcome = cmpfunc(key, A+mid)) < 0) {
		return binary_search(A, lo, mid, key);
	} else if (outcome > 0) {
		return binary_search(A, mid+1, hi, key);
	} else {
		return BS_FOUND;
	}
}
