#include "trie.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void trie_node_destroy(trie_node_t *const trie);
void trie_entry_destroy(trie_entry_t *const trie_entry);
int trie_print_node(trie_node_t *const n, char* prefix, int* cur_len);
int trie_add_node_rec(trie_node_t *const n,
                       char *const v,
                       int start,
                       int len);

int init_trie(trie_t *const trie)
{
  trie_node_t *root = (trie_node_t *)malloc(sizeof(trie_node_t));

  if (root == NULL) {
    fprintf(stderr, "could not allocate memory for root node, aborting...\n");
    return 1;
  }

  root->key = '\0';
  root->entries = NULL;
  root->n_entries = 0;
  root->n_children = 0;
  root->children = NULL;

  trie->root = root;
  trie->nodes = 0;
  trie->values = 0;
  return 0;
}

int trie_add_value(trie_t *const t, char *const v, unsigned int v_len)
{
  trie_node_t *const root = t->root;
  unsigned int idx = 0;

  trie_node_t *cur, *last;
  cur = root;

  while (cur != NULL) {

    int n = cur->n_children;
    last = cur;

    for (int i = 0 ; i < n ; ++i) {
      trie_node_t *const child = &cur->children[i];

      if (child->key == v[idx]) {
        cur = child; /* advance to next in path */
        break;
      }
    }

    if (last == cur) {
      char * intermediate = alloca(sizeof(char) * (v_len - idx + 1));
      memset(intermediate, '\0', v_len - idx + 1);
      memcpy(intermediate, v + idx, v_len - idx);
      break;
    }

    idx++;
  }

  if (v_len - idx == 0) {
    return 0; /* string already exists in the trie */
  }

  return trie_add_node_rec(cur, v, idx, v_len);
}

int trie_add_node_rec(trie_node_t *const n, char *const v, int start, int len)
{

  if (n->n_children == 0) {
    n->children = malloc(sizeof(trie_node_t));
  } else {
    n->children = realloc(n->children, (n->n_children + 1) * sizeof(trie_node_t));
  }

  trie_node_t *const new_child = &n->children[n->n_children];
  new_child->key = v[start];
  new_child->n_children = 0;
  n->entries_on_path += 1;
  n->n_children += 1;

  if (start == (len - 1)) {
    new_child->entries = malloc(sizeof(trie_entry_t));
    new_child->n_entries = 1;
    new_child->entries->next = NULL;
    new_child->entries->value = malloc(MAX_LENGTH * sizeof(char));
    strncpy(new_child->entries->value, v, MAX_LENGTH);
    return 0;
  } else {
    new_child->entries = NULL;
    new_child->n_entries = 0;
  }

  return trie_add_node_rec(new_child, v, start + 1, len);
}

void trie_print(trie_t *const trie)
{
  fprintf(stderr, "==Printing trie==\n");
  fprintf(stderr, "nodes:%lu values:%lu\n", trie->nodes, trie->values);

  char *const prefix = malloc(sizeof(char) * MAX_LENGTH);
  int length = 0;

  if (trie->root != NULL) {
    if (trie->root->children != NULL) {
      for (int i = 0 ; i < trie->root->n_children; ++i) {
        trie_print_node(&trie->root->children[i], prefix, &length);
      }
    }
  }

  free(prefix);

  fprintf(stderr, "==end==\n");
}

int trie_print_node(trie_node_t *const n, char* prefix, int* cur_len)
{
  if (*cur_len > MAX_LENGTH) {
    fprintf(stderr, "cur_len > MAX_LENGTH in trie_print_node, abort\n");
    return 1;
  }

  prefix[(*cur_len)++] = n->key;
  prefix[(*cur_len)++] = '-';

  if (n->n_entries > 0) {
    prefix[(*cur_len)++] = '\0';
    trie_entry_t * entry = n->entries;
    while (entry != NULL) {
      fprintf(stderr, "%s: %s\n", prefix, entry->value);
      entry = entry->next;
    }
    (*cur_len)--;
  }

  if (n->n_children > 0) {
    for (int i = 0 ; i < n->n_children; ++i) {
      trie_print_node(&n->children[i], prefix, cur_len);
    }
  }

  prefix[--(*cur_len)] = '\0';
  prefix[--(*cur_len)] = '\0';

  return 0;
}

void trie_destroy(trie_t *const trie)
{
  if (trie->root != NULL) {
    trie_node_destroy(trie->root);
  }
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
