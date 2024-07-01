#ifndef _TRIE_H_

#define _TRIE_H_
#define READ_BUFFER_SIZE 1024 /* max buffer size when reading file */
#define MAX_LENGTH 200 /* max length is including \0 character */

typedef struct TrieNode {
  struct TrieNode* children;
  struct TrieNode* parent;
  unsigned int n_children;
  unsigned int values_on_path;
  char * value;
  char key;
  unsigned int depth;
} trie_node_t;

typedef struct Trie {
  trie_node_t* root;
  unsigned long nodes;
  unsigned long values;
} trie_t;

void trie_print_prefix(trie_t *const trie);

/*
 * Initializes a region that is allocated for a trie. The user can decide
 * where to store the trie. It will set up basic values to get started using
 * the trie.
 *
 * trie_t *const trie:  pointer to a region in memory allocated for the trie
 */
int init_trie(trie_t *const trie);

/*
 * Destroys the trie that this pointer points to. It frees all memory that
 * is allocated for entries, nodes and the trie itself. The pointer is void
 * after the function call.
 *
 * trie_t *const trie:  the pointer to the trie that needs to be cleaned up
 */
void trie_destroy(trie_t *const trie);

/*
 * Adds a string value to the trie. New nodes are created if required, and
 * existing nodes will be exttended as required.
 *
 * trie *const t:       a pointer to the trie to which the value should be added
 * char *const v:       a pointer to the string value that should be added to
 *                      the trie, this string should be \0 terminated
 * unsigned int v_len:  the length of the string value 'v' without the '\0'
 *                      terminating byte
 */
int trie_add_value(trie_t *const t, char *const v, unsigned int v_len);

/*
 * Print the content of the trie to stderr. Every unique path is printed along
 * with the entry that belongs to that path.
 */
void trie_print(trie_t *const trie);

#endif
