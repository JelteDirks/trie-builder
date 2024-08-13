cmake_minimum_required(VERSION 3.10)
project(buildtrie)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Set the compiler
set(CMAKE_C_COMPILER clang)

# Set the compiler flags
set(CMAKE_C_FLAGS "-Wall -march=armv8-a")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.c
    src/cli.c
    src/trie.c
)

# Header files
set(HEADERS
    src/cli.h
    src/trie.h
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set the output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

