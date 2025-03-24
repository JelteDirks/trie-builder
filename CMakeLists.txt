cmake_minimum_required(VERSION 3.10)
project(CTrie VERSION 0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.c
    src/cli.c
    src/trie.c
)

set(HEADERS
    src/cli.h
    src/trie.h
)

add_executable(buildtrie ${SOURCES} ${HEADERS})
