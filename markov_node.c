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
  for (Node *ptr = first; ptr != NULL; ptr=ptr->next)
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
  if (inside)
  {
    return inside;
  }

  MarkovNode *new_markov = malloc (sizeof (MarkovNode));
  if (new_markov == NULL)
  {
    return NULL;
  }
  *new_markov = (MarkovNode) {data_ptr, NULL};

  if (add (markov_chain->database, new_markov) == 1)
  {
    return NULL;
  }

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
      return 1;
    }
    *new_freq = (MarkovNodeFrequency) {second_node, 1};

    first_node->frequency_list = new_freq;
    first_node->frequency_list_size = 1;
    new_freq = NULL;
    return 0;
  }

  // check if the data already exists in the list
  for (MarkovNodeFrequency* ptr1 = ptr; ptr1 < ptr + size; ptr1++)
  {
    if (strcmp (ptr1->markov_node->data, second_node->data) == 0)
    {
      ptr1->frequency += 1;
      return 0;
    }
  }

  // add a new element to the list
  MarkovNodeFrequency *new_freq = realloc (ptr,
                                           (size + 1)
                                           * sizeof (MarkovNodeFrequency));
  if (new_freq == NULL)
  {
    return 1;
  }

  new_freq[size] = (MarkovNodeFrequency) {second_node, 1};
  first_node->frequency_list = new_freq;
  first_node->frequency_list_size += 1;
  return 0;
}

void free_database (MarkovChain **ptr_chain)
{
  if (ptr_chain == NULL || *ptr_chain == NULL)
  {
    return;
  }

  MarkovChain* chain = *ptr_chain;
  Node* ptr = chain->database->first;

  while(ptr)
  {
    Node* temp = ptr->next;
    MarkovNode* curr_markov = ptr->data;

    if (curr_markov)
    {
      free(curr_markov->data);
      curr_markov->data = NULL;

      MarkovNodeFrequency* curr_freq = curr_markov->frequency_list;
      if (curr_freq)
      {
        free(curr_freq);
        curr_freq = NULL;
      }

      free(curr_markov);
      curr_markov = NULL;
    }

    free(ptr);
    ptr = temp;
  }

  free(chain->database);
  chain->database = NULL;
  free(chain);
  chain = NULL;
}