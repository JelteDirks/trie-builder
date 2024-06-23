#include "trie.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int trie_node_append_entry(trie_entry_t ** base, trie_entry_t* v);
void trie_node_destroy(trie_node_t *const trie);
void trie_entry_destroy(trie_entry_t *const trie_entry);
void trie_print_node(trie_node_t *const n, char* prefix, int cur_len);

int build_trie(trie_t *const trie) {
  trie_node_t *root = (trie_node_t *)malloc(sizeof(trie_node_t));

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

int trie_add_value(trie_t *const t, char *const v, unsigned int v_len)
{
  return 0;
}

int trie_node_append_entry(trie_entry_t ** base, trie_entry_t* v)
{
  return 0;
}

void trie_print(trie_t *const trie)
{
}

void trie_print_node(trie_node_t *const n, char* prefix, int cur_len)
{
}

void trie_destroy(trie_t *const trie)
{
  trie_node_destroy(trie->root);
  free(trie);
}

void trie_node_destroy(trie_node_t *const trie_node)
{
  if (trie_node->entries != NULL) {
    trie_entry_destroy(trie_node->entries);
  }

  if (trie_node->children != NULL) {
    free(trie_node->children);
  }

  free(trie_node);
}

void trie_entry_destroy(trie_entry_t *const trie_entry)
{
  trie_entry_t *cur = trie_entry;
  while (cur != NULL) {
    trie_entry_t *last = cur;
    cur = cur->next;
    free(last);
  }
}
