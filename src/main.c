/* Copyright (C) Jelte Dirks - All Rights Reserved */

#include "cli.h"
#include "trie.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 1024

int main(int argc, char **argv)
{

  char file_path[MAX_ARG_LEN];

  if (handle_args(argc, argv, file_path)) {
    return 1;
  }

  printf("Analyzing file: %s\n", file_path);

  int fd = open(file_path, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "error opening file %s\n", file_path);
    switch (errno) {
    case EACCES:
      printf("access denied\n");
      break;
    }
    return 1;
  }

  char read_buf[READ_BUFFER_SIZE];
  ssize_t bytes_read;
  trie_t *triep = malloc(sizeof(trie_t));

  if (build_trie(triep)) {
    fprintf(stderr, "could not build tree, check stderr for the reason\n");
  }

  while ((bytes_read = read(fd, read_buf, READ_BUFFER_SIZE)) > 0) {
    printf("READ: %s\n", read_buf);
  };

  destroy_trie(triep);
}
