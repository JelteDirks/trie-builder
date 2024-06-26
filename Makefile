OBJS=src/main.o src/cli.o src/trie.o
OUT=buildtrie
CC=clang
CFLAGS=-Wall
OPTFLAGS=-g -O0
DEPS=src/cli.h src/trie.h


all: debug

%.o: %.c $(DEPS)
	$(CC) $(OPTFLAGS) $(CFLAGS) -c -o $@ $<

debug: OPTFLAGS=-g -O0
debug: $(OBJS) $(DEPS)
	$(CC) $(OPTFLAGS) -o $(OUT) $(OBJS) 

release: OPTFLAGS=-O3
release: clean $(OBJS) $(DEPS)
	$(CC) $(OPTFLAGS) -o $(OUT) $(OBJS) 

clean:
	rm -f $(OBJS) $(OUT)
