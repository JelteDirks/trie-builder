# Compile main, plain and simple
OBJS=main.o cli.o
OUT=buildtrie
CC=clang
CFLAGS=-g -Wall
DEPS=cli.h

build: $(OBJS) $(DEPS)
	$(CC) -o $(OUT) $(OBJS) 

clean: $(OBJS) $(OUT)
	rm $(OBJS) $(OUT)
