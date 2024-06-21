#include "trie.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


int build_trie(trie_t *const trie)
{
  trie_node_t* root = (trie_node_t *) malloc(sizeof(trie_node_t));

  if (root == NULL) {
    fprintf(stderr, "could not allocate memory for root node, aborting...\n");
    return 1;
  }

  root->key = '\0';
  root->entries = NULL;
  root->nvalues = 0;
  root->children = NULL;

  trie->root = root;
  trie->nodes = 0;
  trie->values = 0;
  return 0;
}

void destroy_trie(trie_t *const trie)
{
  destroy_trie_node(trie->root);
  free(trie);
}

void destroy_trie_node(trie_node_t *const trie_node)
{
  if (trie_node->entries != NULL) {
    destroy_trie_entries(trie_node->entries);
  }

  if (trie_node->children != NULL) {
    free(trie_node->children);
  }

  free(trie_node);
}

void destroy_trie_entries(trie_entry_t *const trie_entry)
{
  trie_entry_t* cur = trie_entry;
  while (cur != NULL) {
    trie_entry_t* last = cur;
    cur = cur->next;
    free(last);
  }
}
