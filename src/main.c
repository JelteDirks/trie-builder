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


  // TODO: use newline markers to copy into word buffer
  // NOTE: maybe use the existing buffer for the string copy? Replace the newline
  // with a 0 byte and start the string copy right after the last known location.
  char word_buffer[MAX_LENGTH];

  int read_buffer_offset = 0;
  while ((bytes_read = read(fd, &read_buf[read_buffer_offset], READ_BUFFER_SIZE - read_buffer_offset)) > 0) {

    // TODO: maybe only reset when a newline is detected
    read_buffer_offset = 0; /* reset read buffer offset immediately */

    int vector_ops = (bytes_read / 16);
    int remaining_bytes = bytes_read % 16;
    int copy_start = 0;

    int i = 0;
    while (vector_ops--) {
      uint8_t const* idx = (uint8_t const*) &read_buf[i];
      uint8x16_t vector = vld1q_u8(idx);
      uint8x16_t cmp = vceqq_u8(vector, newlines);

      uint8_t newlines_exists = vaddvq_u8(cmp);

      if (newlines_exists) {
        int first_newline = extract_lowest_logical(cmp);
        int newline_offset = i + first_newline;

        read_buf[newline_offset] = '\0';

        strncpy(word_buffer, &read_buf[copy_start], MAX_LENGTH - 1);
        word_buffer[MAX_LENGTH - 1] = '\0';

        printf("line: |%s|\n", word_buffer);

        copy_start = newline_offset + 1;

        /* redo, in case there are 2 newlines in the same vector*/
        vector_ops++;
        i-= 16;
      }

      i += 16;
    }

    for (i = 0; i < remaining_bytes; ++i) {
      if (read_buf[i] == '\n') {
        read_buf[i] = '\0';
        strncpy(word_buffer, &read_buf[copy_start], MAX_LENGTH -1);
        word_buffer[MAX_LENGTH - 1] = '\0';
        copy_start = i + 1;
        printf("line: |%s|\n", word_buffer);
      }
    }

    if (copy_start < bytes_read) {

      size_t remaining = bytes_read - copy_start;
      memcpy(read_buf, &read_buf[copy_start], remaining);
      read_buffer_offset = (int) remaining;

      printf("bytes are left\n");
      printf("copy start is at %d\n", copy_start);
      printf("bytes read is at %ld\n", bytes_read);

      // TODO: make sure that the last part of this buffer is not skipped.
      // Copy last part of buffer to the first part of next buffer and set
      // read buffer offset.
    }
  };


  int status = close(fd);
  if (status == -1) {
    fprintf(stderr, "error closing the file\n");
    return 1;
  }
}
