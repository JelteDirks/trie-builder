/* Copyright (C) Jelte Dirks - All Rights Reserved */

#include "cli.h"
#include "trie.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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
  char word_buf[MAX_LENGTH];
  ssize_t bytes_read;
  trie_t *triep = malloc(sizeof(trie_t));

  if (init_trie(triep)) {
    fprintf(stderr, "could not build tree, check stderr for the reason\n");
  }

  char c = '\0';
  while ((bytes_read = read(fd, &c, 1)) == 1) {
    if (c == '\r') {
      fprintf(stderr, "dos line endings detected, this is not supported yet, aborting...\n");
      return 1;
    }
    if (c == '\n') {
      break; /* no need to scan the entire file, \r preceeds \n */
    }
  }

  off_t new_offset = lseek(fd, 0, SEEK_SET);
  if (new_offset != 0) {
    fprintf(stderr, "error setting file offset to start of file\n");
    return 1;
  }

  int line_start_offset;
  size_t length;
  unsigned int i;
  unsigned int word_offset = 0;

  while ((bytes_read = read(fd, read_buf, READ_BUFFER_SIZE)) > 0) {
    line_start_offset = -1;
    i = 0;

    while (i < bytes_read) {
      char c = read_buf[i];
      switch (c) {
        case '\n':
          length = i - line_start_offset - 1;

          if (length > (MAX_LENGTH - 1)) {
            fprintf(stderr, "length of the next line (=%ld) exceeds max length (=%d)\n",
                    length,
                    MAX_LENGTH);
            return 1;
          }

          if (word_offset > 0) {
            memcpy(word_buf + (word_offset - 1) * sizeof(char),
                   read_buf + line_start_offset + 1,
                   length * sizeof(char));
            word_buf[word_offset + length - 1] = '\0';
            trie_add_value(triep, word_buf, word_offset + length - 1);
          } else {
            memcpy(word_buf, read_buf + line_start_offset + 1,
                   length * sizeof(char));
            word_buf[length] = '\0';
            trie_add_value(triep, word_buf, length);
          }


          line_start_offset = i;
          word_offset = 0;
          break;
      }

      ++i;
    }

    word_offset = i - line_start_offset;
    memcpy(word_buf, read_buf + line_start_offset + 1, word_offset * sizeof(char));
  };

  int status = close(fd);
  if (status == -1) {
    fprintf(stderr, "error closing the file\n");
    return 1;
  }

  trie_print_prefix(triep);
  trie_destroy(triep);
  free(triep);
}
