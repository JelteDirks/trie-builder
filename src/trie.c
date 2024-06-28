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

  if (node == NULL) return 1;

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

trie_node_t* get_last_child(trie_node_t *const node)
{
  if (node->n_children == 0 || node->children == NULL) {
    fprintf(stderr, "node has 0 children or NULL pointer in get_last_child\n");
    return NULL;
  }
  return &node->children[node->n_children - 1];
}

int increase_children_arr(trie_node_t *const node)
{
  if (node == NULL) {
    fprintf(stderr, "node=NULL in increase_children_arr\n");
    return 1;
  }
  node->n_children += 1;
  trie_node_t *newarr = realloc(node->children, sizeof(trie_node_t) * node->n_children);
  if (newarr == NULL) return 1;
  node->children = newarr;
  if (init_trie_node(get_last_child(node))) {
    fprintf(stderr, "failed to increase children of node: key=%c value=%s nc=%lu\n",
            node->key,
            node->value,
            node->n_children);
    return 1;
  }
  return 0;
}

int append_empty_child(trie_node_t *const node)
{
  if (node == NULL) {
    fprintf(stderr, "node=NULL in append_empty_child\n");
    return 1;
  }
  if (node->children == NULL || node->n_children == 0) {
    trie_node_t *newnode = create_empty_node();
    if (newnode == NULL) return 1;
    node->children = newnode;
    node->n_children = 1;
    return 0;
  }
  return increase_children_arr(node);
}

trie_node_t* append_child_with_key(trie_node_t *const node, char key)
{
  if (append_empty_child(node)) {
    return NULL;
  }

  trie_node_t* lastchild = get_last_child(node);
  lastchild->key = key;

  return lastchild;
}

int copy_value(trie_node_t *const node, char *value)
{
  size_t len = strnlen(value, MAX_LENGTH);
  if (len > MAX_LENGTH) {
    fprintf(stderr, "length exceeds max length: %s\n", value);
    return 1;
  }
  node->value = (char *) malloc(sizeof(char) * (len + 1));
  strncpy(node->value, value, len);
  node->value[len] = '\0';

  return 0;
}

int append_new_chain(trie_node_t *const base, char *const value, unsigned int start, unsigned int length)
{
  unsigned int pos = start;
  trie_node_t *cur = base;

  while (pos < length) {
    trie_node_t* newchild = append_child_with_key(cur, value[pos]);
    if (newchild == NULL) {
      return 1;
    }
    cur = newchild;
    pos++;
  }

  return copy_value(cur, value);
}

int trie_add_value(trie_t *const t, char *const v, unsigned int v_len)
{
  trie_node_t * cur = t->root;
  unsigned int pos = 0;

  while (pos < v_len) {
    if (cur->children == NULL || cur->n_children == 0) {
      return append_new_chain(cur, v, pos, v_len);
    } else {
      trie_node_t *found = NULL;
      for (int i = 0; i < cur->n_children; i++) {
        trie_node_t *child = &cur->children[i];
        if (child->key == v[pos]) {
          found = child;
        }
      }
      if (found == NULL) {
        return append_new_chain(cur, v, pos, v_len);
      } else {
        cur = found;
        pos++;
      }
    }
  }

  return copy_value(cur, v);
}

void trie_destroy(trie_t *const trie)
{
  // TODO: clean up all memory from this trie.
}
