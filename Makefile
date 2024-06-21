# Compile main, plain and simple
OBJS=src/main.o src/cli.o src/trie.o
OUT=buildtrie
CC=clang
CFLAGS=-g -Wall
DEPS=src/cli.h

build: $(OBJS) $(DEPS)
	$(CC) -o $(OUT) $(OBJS) 

clean: $(OBJS) $(OUT)
	rm $(OBJS) $(OUT)
