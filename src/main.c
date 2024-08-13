/* Copyright (C) Jelte Dirks - All Rights Reserved */


#include "cli.h"
#include "trie.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arm_neon.h>

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

  uint8x16_t newlines = {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};

  // TODO: use newline markers to copy into word buffer
  // NOTE: maybe use the existing buffer for the string copy? Replace the newline
  // with a 0 byte and start the string copy right after the last known location.
  ssize_t previous_newline = 0;
  char word_buffer[MAX_LENGTH];

  while ((bytes_read = read(fd, read_buf, READ_BUFFER_SIZE)) > 0) {
    int vector_ops = (bytes_read / 16);
    int remaining_bytes = bytes_read % 16;

    ssize_t i = 0;
    while (vector_ops--) {
      uint8_t const* idx = (uint8_t const*) &read_buf[i];
      uint8x16_t vector = vld1q_u8(idx);
      uint8x16_t cmp = vceqq_u8(vector, newlines);

      uint8_t newlines_exists = vaddvq_u8(cmp);

      if (newlines_exists) {
        int first_newline = extract_lowest_logical(cmp);
        printf("first_newline: %d\n", first_newline);
      }

      i += 16;
    }

    if (remaining_bytes) {
      printf("remainging_bytes to analyse: %d\n", remaining_bytes);
    }

    printf("bytes_read: %ld\n", bytes_read);

    break;
  };


  int status = close(fd);
  if (status == -1) {
    fprintf(stderr, "error closing the file\n");
    return 1;
  }

  printf("Done\n");
}
