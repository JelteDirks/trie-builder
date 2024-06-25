/* Copyright (C) Jelte Dirks - All Rights Reserved */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "cli.h"

void print_usage(void);

int handle_args(int argc, char **argv, char* file_path)
{
  if (argc < 2) {
    fprintf(stderr, "There is no file provided as input. Please refer to the usage.\n");
    print_usage();
    return 1;
  }

  if (strncmp("-h", argv[1], 3) == 0) {
    print_usage();
    return 1;
  }

  size_t filename_len = strnlen(argv[1], MAX_ARG_LEN);

  if (filename_len == MAX_ARG_LEN && argv[1][MAX_ARG_LEN] != '\0') {
    fprintf(stderr, "Filename is too long, it exceeds %u characters\n", MAX_ARG_LEN);
    return 1;
  }

  strncpy(file_path, argv[1], MAX_ARG_LEN);

  struct stat filestats;
  if (stat(file_path, &filestats) == -1) {
    fprintf(stderr, "Problem reading file %s, check if it exists\n", file_path);
    return 1;
  }

  if (!S_ISREG(filestats.st_mode)) {
    fprintf(stderr, "File is not a regular file, please provide an ASCII file\n");
    fprintf(stderr, "file type: ");
    switch (filestats.st_mode & S_IFMT) {
      case S_IFDIR: fprintf(stderr,"directory\n"); break;
      case S_IFBLK: fprintf(stderr,"block device\n"); break;
      case S_IFCHR: fprintf(stderr,"character device\n"); break;
      case S_IFIFO: fprintf(stderr,"FIFO or pipe\n"); break;
      case S_IFSOCK: fprintf(stderr,"Socket\n"); break;
      case S_IFLNK: fprintf(stderr,"Symbolic link\n"); break;
    }
    return 1;
  }

  return 0;
}

void print_usage(void) 
{
  fprintf(stderr, "usage: ./buildtrie [SOURCE_FILE]\n");
  fprintf(stderr, "SOURCE_FILE: File that contains lines that should be converted to a Trie.\n");
  fprintf(stderr, "             The maximum length of the name is 200 characters.\n");
}
