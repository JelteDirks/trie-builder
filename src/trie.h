#ifndef _TRIE_H_

#define _TRIE_H_

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
void destroy_trie(trie_t *const trie);
void destroy_trie_node(trie_node_t *const trie);
void destroy_trie_entries(trie_entry_t *const trie_entry);

#endif
