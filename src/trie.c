#include "trie.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_node(trie_node_t *node)
{
  fprintf(stderr, "n_children:%u value:%s values_on_path:%u key:%c depth:%u\n",
          node->n_children,
          node->value,
          node->values_on_path,
          node->key,
          node->depth);
}
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
  node->depth = 0;

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

void update_parent(trie_node_t *const node) {
  for (int i = 0; i < node->n_children; i++) {
    trie_node_t *const child = &node->children[i];
    child->parent = node;
    for (int j = 0; j < child->n_children; j++) {
      trie_node_t *const childchild = &child->children[j];
      childchild->parent = child;
    }
  }
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
    fprintf(stderr, "failed to increase children of node: key=%c value=%s nc=%u\n",
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
    node->children->parent = node;
    node->n_children = 1;
    return 0;
  }

  increase_children_arr(node);
  return 0;
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

int append_new_chain(trie_node_t *const base,
                     char *const value,
                     unsigned int start,
                     unsigned int length,
                     unsigned int depth)
{
  unsigned int pos = start;
  trie_node_t *cur = base;

  while (pos < length) {
    cur->values_on_path += 1;
    trie_node_t* newchild = append_child_with_key(cur, value[pos]);
    if (newchild == NULL) {
      fprintf(stderr, "failed to append new child in chain\n");
      return 1;
    }
    newchild->depth = depth;
    cur = newchild;
    depth += 1;
    pos++;
  }

  cur->values_on_path = 1;

  return copy_value(cur, value);
}

int trie_add_value(trie_t *const t, char *const v, unsigned int v_len)
{
  trie_node_t * cur = t->root;
  unsigned int pos = 0;
  unsigned int depth = 0;

  while (pos < v_len) {
    if (cur->children == NULL || cur->n_children == 0) {
      return append_new_chain(cur, v, pos, v_len, depth + 1);
    } else {
      trie_node_t *found = NULL;
      for (int i = 0; i < cur->n_children; i++) {
        trie_node_t *child = &cur->children[i];
        if (child->key == v[pos]) {
          found = child;
        }
      }
      if (found == NULL) {
        return append_new_chain(cur, v, pos, v_len, depth + 1);
      } else {
        cur->values_on_path += 1;
        depth += 1;
        cur = found;
        pos++;
      }
    }
  }

  cur->values_on_path += 1;

  return copy_value(cur, v);
}

void verify_parent_node(trie_node_t *const node)
{
  for (int i = 0; i < node->n_children; i++) {
    verify_parent_node(&node->children[i]);
    assert(node->children[i].parent == node);
  }
}

void verify_values_on_path(trie_node_t *const node)
{
  unsigned int values = 0;
  const unsigned int vop = node->values_on_path;

  if (node->value != NULL) {
    values += 1;
  }

  for (int i = 0; i < node->n_children; i++) {
    verify_values_on_path(&node->children[i]);
    values += node->children[i].values_on_path;
  }

  assert(values == vop);
}

void verify_depth(trie_node_t *const node, unsigned int depth)
{
  assert(node->depth == depth);
  for (int i = 0; i < node->n_children; i++) {
    verify_depth(&node->children[i], depth + 1);
  }
}

void print_node_tree(trie_node_t *const node, char * prefix)
{
  if (node->key == '\0') {
    prefix[node->depth * 2] = 'r';
  } else {
    prefix[node->depth * 2] = node->key;
  }
  prefix[node->depth * 2 + 1] = '-';
  prefix[node->depth * 2 + 2] = '\0';


  if (node->value != NULL) {
    fprintf(stderr, "%s: %s\n" , prefix, node->value);
  }

  for (int i = 0; i < node->n_children; i++) {
    print_node_tree(&node->children[i], prefix);
  }

  prefix[node->depth * 2] = '\0';
  prefix[node->depth * 2 + 1] = '\0';
}

void trie_print(trie_t *const trie)
{
  verify_depth(trie->root, 0);
  verify_values_on_path(trie->root);

  char buffer[1000];
  memset(buffer, 0, 1000);
  print_node_tree(trie->root, buffer);
  fprintf(stderr, "\n");
}

void destroy_node(trie_node_t *const node)
{
  for (int i = 0; i < node->n_children; i++) {
    destroy_node(&node->children[i]);
  }

  free(node->value);
  free(node->children);
}

void trie_destroy(trie_t *const trie)
{
  destroy_node(trie->root);
}
