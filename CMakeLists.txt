cmake_minimum_required(VERSION 3.10)
project(buildtrie VERSION 0.1 LANGUAGES C)

set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_ASM_FLAGS "-mfpu=neon -mfloat-abi=hard")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)

set(SOURCES
    src/main.c
    src/cli.c
    src/trie.c
)

set(HEADERS
    src/cli.h
    src/trie.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set the output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

