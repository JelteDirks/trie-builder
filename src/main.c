/* Copyright (C) Jelte Dirks - All Rights Reserved */

#include "cli.h"
#include "trie.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arm_neon.h>

#define ELINES_TOO_LONG 1
#define EFILE_CLOSE 2
#define EFILE_OPEN 3
#define ETRIE_ERROR 4

static const uint8x16_t newlines = {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};

int extract_lowest_logical(uint8x16_t src)
{
  uint8_t local[16];
  vst1q_u8(local, src);

  for (int i = 0; i < 16; i++) {
    if (local[i] == 255) {
      return i;
    }
  }

  return -1;
}

int main(int argc, char **argv)
{
  char file_path[MAX_ARG_LEN];

  if (handle_args(argc, argv, file_path)) {
    return 1;
  }

  int fd = open(file_path, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "error opening file %s\n", file_path);
    switch (errno) {
    case EACCES:
      printf("access denied\n");
      break;
    }
    return EFILE_OPEN;
  }

  char read_buf[READ_BUFFER_SIZE];
  ssize_t bytes_read;

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

  trie_t *triep = malloc(sizeof(trie_t));

  if (init_trie(triep)) {
    fprintf(stderr, "could not build tree, check stderr for the reason\n");
    return ETRIE_ERROR;
  }

  int read_offset = 0;

  while ((bytes_read = read(fd, &read_buf[read_offset], READ_BUFFER_SIZE - read_offset)) > 0) {

    read_buf[read_offset + bytes_read] = '\0';
    read_offset = 0;

    int copy_start = 0;
    int vector_ops = (bytes_read / 16);
    int i = 0;

    while (vector_ops--) {
      uint8x16_t vector = vld1q_u8((uint8_t const*) &read_buf[i]);
      uint8x16_t cmp = vceqq_u8(vector, newlines);
      uint8_t newlines_exists = vaddvq_u8(cmp);

      if (newlines_exists) {
        for (int j = 0; j < 16; ++j) {
          int offset = i + j;
          if (read_buf[offset] == '\n') {
            read_buf[offset] = '\0';
            trie_add_value(triep, &read_buf[copy_start], offset - copy_start);
            copy_start = offset + 1;
          }
        }
      }

      i += 16;
    }

    for (; i < bytes_read; ++i) {
      if (read_buf[i] == '\n') {
        read_buf[i] = '\0';
        trie_add_value(triep, &read_buf[copy_start], i - copy_start);
        copy_start = i + 1;
      }
    }

    if (copy_start == 0) {
      fprintf(stderr, "the buffer size exceeds the longest line, this is not supported\n");
      return ELINES_TOO_LONG;
    }

    const size_t remainder = bytes_read - copy_start;
    if (remainder) {
      memcpy(read_buf, &read_buf[copy_start], remainder);
      read_offset = remainder;
    }
  };

  int status = close(fd);
  if (status == -1) {
    fprintf(stderr, "error closing the file\n");
    return EFILE_CLOSE;
  }

  //verify_trie(triep);

  trie_print_prefix(triep);
  trie_destroy(triep);
  free(triep);
}
