#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"

#define FULL_FILE (-1)
#define BUFFER_SIZE 1002

int check_dot (const char *word)
{
  if (word[strlen (word) - 1] == '.')
  {
    return 0;
  }
  return 1;
}

int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char buffer[BUFFER_SIZE];

  int k = 0;
  MarkovNode *prev = NULL;
  while ((fgets (buffer, BUFFER_SIZE, fp) != NULL) && (words_to_read >= k ||
                                                       words_to_read
                                                       == FULL_FILE))
  {
    char *token = strtok (buffer, DELIMITERS);

    while (token != NULL && (words_to_read >= k ||
                             words_to_read == FULL_FILE))
    {
      Node *new_word = add_to_database (markov_chain, token);
      if (new_word == NULL)
      {
        return EXIT_FAILURE;
      }
      if (words_to_read != FULL_FILE)
      {
        k += 1;
      }
      if (prev != NULL)
      {
        if (add_node_to_frequency_list (prev,
                                        new_word->data) == 1)
        {
          return EXIT_FAILURE;
        }
      }
      prev = new_word->data;
      if (check_dot (token) == 0)
      {
        prev = NULL;
      }
//      printf ("Token: %s\nLast element: %s\n", token,
//              markov_chain->database->last->data->data);
      token = strtok (NULL, DELIMITERS);
    }
  }
  return EXIT_SUCCESS;
}

int main (int argc, char *argv[])
{
  if (argc < 4 || argc > 5)
  {
    fprintf (stdout, "%s", NUM_ARGS_ERROR);
    return EXIT_FAILURE;
  }

  FILE *corpus = fopen (argv[3], "r");
  if (corpus == NULL)
  {
    fprintf (stdout, "%s", FILE_PATH_ERROR);
    return EXIT_FAILURE;
  }

  LinkedList *database = malloc (sizeof (LinkedList));
  if (database == NULL)
  {
    fclose (corpus);
    fprintf (stdout, "%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  *database = (LinkedList) {NULL, NULL, 0};

  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if (markov_chain == NULL)
  {
    fclose (corpus);
    free (database);
    database = NULL;
    fprintf (stdout, "%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  *markov_chain = (MarkovChain) {database};

  int words_to_read = FULL_FILE;
  if (argc == 5)
  {
    words_to_read = strtol (argv[4], NULL, 10);
  }
  if (fill_database (corpus, words_to_read, markov_chain) == 1)
  {
    fclose (corpus);
    free_database (&markov_chain);
    return EXIT_FAILURE;
  }
  // add fill_database call with relevant memory allocation and other logic ...
  fclose (corpus);

//  Node* temp = database->first;
//  while (temp != NULL)
//  {
//    printf("%s\n", temp->data->data);
//    temp = temp->next;
//  }
//  printf ("%s\n%s", markov_chain->database->first->data->data,
//          markov_chain->database->last->data->data);

  int num_of_tweets = strtol(argv[2], NULL, 10);
  for (int i = 0; i < num_of_tweets; i++)
  {
    MarkovNode* first = get_first_random_node (markov_chain);

    fprintf (stdout, "Tweet %d: ", i + 1);
    generate_tweet (first, 20);
  }

//  free_database (&markov_chain);
  return EXIT_SUCCESS;
}

