#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"

#define FULL_FILE (-1)

int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char buffer[1002];
  if (words_to_read == FULL_FILE)
  {
    int k = 0;
    while (fgets (buffer, 1002, fp) != NULL)
    {
      char *token = strtok (buffer, DELIMITERS);

      Node *prev = NULL;
      while (token != NULL)
      {
        char *data = malloc (strlen (token) * sizeof (char));
        strcpy (data, token);
        Node *new_word = add_to_database (markov_chain, data);
        if (new_word == NULL)
        {
          return EXIT_FAILURE;
        }

        if (prev != NULL && prev->data->data[strlen
                                                 (prev->data->data) - 1]
                            != '.')
        {
          if (add_node_to_frequency_list (prev->data,
                                          new_word->data) == 1)
          {
            return EXIT_FAILURE;
          };
        }

        prev = new_word;
        token = strtok (NULL, DELIMITERS);
      }
      k += 1;
      printf ("%d\n", k);
    }
  }
  else
  {
    int k = 0;
    while (fgets (buffer, 1002, fp) != NULL && k < words_to_read)
    {
      char *token = strtok (buffer, DELIMITERS);

      Node *prev = NULL;
      while (token != NULL && k < words_to_read)
      {
        char *data = malloc (strlen (token) * sizeof (char));
        strcpy (data, token);
        Node *new_word = add_to_database (markov_chain, data);
        if (new_word == NULL)
        {
          return EXIT_FAILURE;
        }
        k += 1;

        if (prev != NULL && prev->data->data[strlen
                                                 (prev->data->data) - 1]
                            != '.')
        {
          if (add_node_to_frequency_list (prev->data,
                                          new_word->data) == 1)
          {
            return EXIT_FAILURE;
          };
        }

        prev = new_word;
        token = strtok (NULL, DELIMITERS);
      }
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
    return EXIT_FAILURE;
  }
  *database = (LinkedList) {NULL, NULL, 0};

  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if (markov_chain == NULL)
  {
    fclose (corpus);
    free (database);
    database = NULL;
    return EXIT_FAILURE;
  }
  *markov_chain = (MarkovChain) {database};

  if (argc == 5)
  {
    int words_to_read = strtol (argv[4], NULL, 10);
    if (fill_database (corpus, words_to_read, markov_chain) == 1)
    {
      fclose (corpus);
      free_database (&markov_chain);
      return EXIT_FAILURE;
    }
  }
  else
  {
    if (fill_database (corpus, FULL_FILE, markov_chain) == 1)
    {
      fclose (corpus);
      free_database (&markov_chain);
      return EXIT_FAILURE;
    }
  }
  // add fill_database call with relevant memory allocation and other logic ...
  fclose (corpus);
  printf ("%s\n%s", markov_chain->database->first->data->data,
          markov_chain->database->last->data->data);
  return EXIT_SUCCESS;
}

