#include "markov_chain.h"

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number (int max_number)
{
  return rand () % max_number;
}

Node *get_node_from_database (MarkovChain *markov_chain, char *data_ptr)
{
  Node *first = markov_chain->database->first;
  for (Node *ptr = first; ptr != NULL; ptr = ptr->next)
  {
    if (strcmp (ptr->data->data, data_ptr) == 0)
    {
      return ptr;
    }
  }

  return NULL;
}

Node *add_to_database (MarkovChain *markov_chain, char *data_ptr)
{

  Node *inside = get_node_from_database (markov_chain, data_ptr);
  if (inside != NULL)
  {
    return inside;
  }

  MarkovNode *new_markov = malloc (sizeof (MarkovNode));
  if (new_markov == NULL)
  {
    fprintf (stdout, "%s", ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }

  char *new_data = malloc (sizeof (char) * strlen (data_ptr));
  if (new_data == NULL)
  {
    fprintf (stdout, "%s", ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }

  strcpy (new_data, data_ptr);
  *new_markov = (MarkovNode) {new_data, NULL, 0};

  if (add (markov_chain->database, new_markov) == 1)
  {
    fprintf (stdout, "%s", ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
//  return get_node_from_database (markov_chain, data_ptr);
  return markov_chain->database->last;
}

int
add_node_to_frequency_list (MarkovNode *first_node, MarkovNode *second_node)
{
  MarkovNodeFrequency *ptr = first_node->frequency_list;
  int size = first_node->frequency_list_size;

  if (ptr == NULL)  // check if frequency_list is empty
  {
    MarkovNodeFrequency *new_freq = malloc (sizeof (MarkovNodeFrequency));
    if (new_freq == NULL)
    {
      fprintf (stdout, "%s", ALLOCATION_ERROR_MASSAGE);
      return 1;
    }
    *new_freq = (MarkovNodeFrequency) {second_node, 1};

    first_node->frequency_list = new_freq;
    first_node->frequency_list_size = 1;
    return 0;
  }

  // check if the data already exists in the list
  for (MarkovNodeFrequency *ptr1 = ptr; ptr1 < ptr + size; ptr1++)
  {
    if (strcmp (ptr1->markov_node->data, second_node->data) == 0)
    {
      ptr1->frequency++;
      return 0;
    }
  }

  // add a new element to the list
  MarkovNodeFrequency *new_freq = realloc (ptr,
                                           (size + 1)
                                           * sizeof (MarkovNodeFrequency));
  if (new_freq == NULL)
  {
    fprintf (stdout, "%s", ALLOCATION_ERROR_MASSAGE);
    return 1;
  }

  new_freq[size] = (MarkovNodeFrequency) {second_node, 1};
  first_node->frequency_list = new_freq;
  first_node->frequency_list_size++;
  new_freq = NULL;
  return 0;
}

void free_database (MarkovChain **ptr_chain)
{
  if (ptr_chain == NULL || *ptr_chain == NULL)
  {
    return;
  }

  MarkovChain *chain = *ptr_chain;
  Node *ptr = chain->database->first;

  while (ptr != NULL)
  {
    Node *temp = ptr->next;
    MarkovNode *curr_markov = ptr->data;

    if (curr_markov != NULL)
    {
//      free (curr_markov->data);
//      curr_markov->data = NULL;

      MarkovNodeFrequency *curr_freq = curr_markov->frequency_list;
      if (curr_freq != NULL)
      {
        free (curr_freq);
        curr_freq = NULL;
      }

      free (curr_markov);
      curr_markov = NULL;
    }

    free (ptr);
    ptr = temp;
  }

  free (chain->database);
  chain->database = NULL;
  free (chain);
  chain = NULL;
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  while (true)
  {
    int num = get_random_number (markov_chain->database->size);
    Node *ptr = markov_chain->database->first;
    for (int i = 0; i < num; i++)
    {
      ptr = ptr->next;
    }
    char *word = ptr->data->data;
    if (word[strlen (word) - 1] != '.')
    {
      return ptr->data;
    }
  }
}

MarkovNode *get_next_random_node (MarkovNode *cur_markov_node)
{
  int num = get_random_number (cur_markov_node->frequency_list_size);
  int k = 0;
  MarkovNodeFrequency *freq = cur_markov_node->frequency_list;
  while (k < num)
  {
    for (int i = 0; i < freq->frequency; i++)
    {
      if (k >= num)
      {
        return freq->markov_node;
      }
      k++;
    }

    freq++;
  }

  return freq->markov_node;
}

void generate_tweet (MarkovNode *first_node, int max_length)
{
  fprintf (stdout, "%s", first_node->data);
  MarkovNode *node = first_node;
  for (int i = 1; i < max_length; i++)
  {
    node = get_next_random_node (node);
    char *word = node->data;
    if (word[strlen (word) - 1] == '.' || i == max_length - 1)
    {
      fprintf (stdout, " %s\n", word);
      return;
    }

    fprintf (stdout, " %s", word);
  }
}