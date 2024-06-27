#include "trie.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Sets initialization values on a trie node. The initial values never acquire
 * any memory, so if it errors the node can be freed if required.
 */
int init_trie_node(trie_node_t * node)
{
  node->children = NULL;
  node->n_children = 0;
  node->value = NULL;
  node->values_on_path = 0;
  node->key = '\0';

  return 0;
}

trie_node_t* create_empty_node()
{
  trie_node_t *node = (trie_node_t *) malloc(sizeof(trie_node_t));
  if (node == NULL) {
    return NULL;
  }

  if (init_trie_node(node)) {
    free(node);
    return NULL;
  }

  return node;
}

int init_trie(trie_t *const trie)
{
  trie_node_t *root = create_empty_node();

  if (root == NULL) {
    fprintf(stderr, "issue creating root node\n");
    return 1;
  }

  trie->root = root;

  return 0;
}

int trie_add_value(trie_t *const t, char *const v, unsigned int v_len)
{
  printf("adding value to trie: %s\n", v);
  return 0;
}

