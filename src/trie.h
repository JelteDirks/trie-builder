#ifndef _TRIE_H_

#define _TRIE_H_
#define READ_BUFFER_SIZE 1024
#define MAX_LENGTH 200

typedef struct TrieEntry {
  struct TrieEntry* next;
  char* value;
} trie_entry_t;

typedef struct TrieNode {
  char key;
  trie_entry_t* entries;
  struct TrieNode* children;
  unsigned long nvalues;
} trie_node_t;

typedef struct Trie {
  trie_node_t* root;
  unsigned long nodes;
  unsigned long values;
} trie_t;

int build_trie(trie_t *const trie);
void trie_destroy(trie_t *const trie);
int trie_add_value(trie_t *const t, char *const v, unsigned int v_len);
void trie_print(trie_t *const trie);

#endif
