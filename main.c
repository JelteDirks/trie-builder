/* Copyright (C) Jelte Dirks - All Rights Reserved */

#include "cli.h"

int main(int argc, char** argv) 
{

  if (handle_args(argc, argv)) {
    return 1;
  }
}
